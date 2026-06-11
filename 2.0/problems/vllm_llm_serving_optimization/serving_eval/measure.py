"""Orchestration: build, serve, and measure baseline vs patched vLLM.

To honour the one-L40S-per-environment budget, the baseline (vanilla vLLM) and
the patched build are never served at the same time. The baseline is read from a
cache baked into the judge image when available; otherwise it is measured once
(serving the clean tree on its own) and cached. The patched build is then served
on its own, gated on greedy-output correctness against the cached baseline
outputs, and measured under the identical workload and arrival schedule.
"""

from __future__ import annotations

import json
import shutil
import subprocess
import tempfile
import uuid
from pathlib import Path
from typing import Any

from .accuracy import compute_accuracy
from .agent_runner import InstanceResult, run_workload
from .correctness import collect_greedy_outputs, compare_outputs
from .scoring import provisional_score
from .sandbox import docker_available
from .serving import ServerHandle, ServingError, deploy_server, stop_server, wait_healthy
from .settings import EvalSettings


def _short_id() -> str:
    return uuid.uuid4().hex[:10]


def _latency_map(results: list[InstanceResult]) -> dict[str, float]:
    return {result.instance_id: result.latency_seconds for result in results}


def _apply_patch(clean_source: str, patch_path: str) -> Path:
    tmp_root = Path(tempfile.mkdtemp(prefix="vllm-serving-opt-src-"))
    patched = tmp_root / "vllm"
    # Keep .git: vLLM's build uses setuptools_scm for versioning, and applying the
    # patch to a tracked tree leaves the changes in the working tree (an editable
    # install then picks them up). The patch is applied with `git apply` below.
    shutil.copytree(clean_source, patched, dirs_exist_ok=False)
    patch_text = Path(patch_path).read_text(encoding="utf-8", errors="replace")
    if patch_text.strip():
        check = subprocess.run(
            ["git", "apply", "--check", patch_path],
            cwd=str(patched),
            capture_output=True,
            text=True,
        )
        if check.returncode != 0:
            shutil.rmtree(tmp_root, ignore_errors=True)
            raise ServingError("patch does not apply cleanly to the pinned vLLM source")
        subprocess.run(["git", "apply", patch_path], cwd=str(patched), check=True, capture_output=True, text=True)
    return patched


def _serve(src: str, settings: EvalSettings, *, app_name: str, label: str) -> ServerHandle:
    handle = deploy_server(
        src_path=src,
        model=settings.model,
        gpu=settings.gpu,
        app_name=app_name,
        label=label,
        scaledown_seconds=settings.modal_scaledown_seconds,
        startup_timeout_seconds=settings.modal_startup_timeout_seconds,
        build_timeout_seconds=settings.build_timeout_seconds,
        deploy_retries=settings.modal_deploy_retries,
    )
    wait_healthy(handle, model=settings.model, timeout_seconds=settings.server_health_timeout_seconds)
    return handle


def _measure_server(
    handle: ServerHandle,
    settings: EvalSettings,
    *,
    role: str,
    accuracy_mode: str,
    prefer_docker: bool,
    greedy_n: int,
    run_id: str,
) -> dict[str, Any]:
    greedy = collect_greedy_outputs(handle.base_url, settings=settings, n=greedy_n)
    results = run_workload(
        base_url=handle.base_url,
        settings=settings,
        role=role,
        prefer_docker=prefer_docker,
    )
    accuracy = compute_accuracy(results, settings=settings, mode=accuracy_mode, run_id=run_id)
    return {
        "per_instance_latency": _latency_map(results),
        "accuracy": float(accuracy["accuracy"]),
        "accuracy_mode": accuracy["mode"],
        "accuracy_proxy_used": bool(accuracy["proxy_used"]),
        "greedy_outputs": greedy,
        "n_instances": len(results),
    }


def _load_baseline_cache(path: str, role: str) -> dict[str, Any] | None:
    cache_path = Path(path)
    if not cache_path.exists():
        return None
    try:
        payload = json.loads(cache_path.read_text(encoding="utf-8"))
    except Exception:
        return None
    if not isinstance(payload, dict):
        return None
    entry = payload.get(role)
    return entry if isinstance(entry, dict) else None


def _store_baseline_cache(path: str, role: str, entry: dict[str, Any]) -> None:
    cache_path = Path(path)
    try:
        cache_path.parent.mkdir(parents=True, exist_ok=True)
        payload: dict[str, Any] = {}
        if cache_path.exists():
            existing = json.loads(cache_path.read_text(encoding="utf-8"))
            if isinstance(existing, dict):
                payload = existing
        payload[role] = entry
        cache_path.write_text(json.dumps(payload), encoding="utf-8")
    except Exception:
        pass


def _get_baseline(
    clean_source: str,
    settings: EvalSettings,
    *,
    role: str,
    accuracy_mode: str,
    baseline_cache_path: str,
    prefer_docker: bool,
) -> dict[str, Any]:
    cached = _load_baseline_cache(baseline_cache_path, role)
    if cached and cached.get("per_instance_latency"):
        return cached

    app_name = f"vllm-serv-opt-base-{role}-{_short_id()}"
    handle = _serve(clean_source, settings, app_name=app_name, label=app_name)
    try:
        measured = _measure_server(
            handle,
            settings,
            role=role,
            accuracy_mode=accuracy_mode,
            prefer_docker=prefer_docker,
            greedy_n=settings.correctness_smoke_prompts,
            run_id=f"baseline-{role}-{_short_id()}",
        )
    finally:
        stop_server(app_name)
    _store_baseline_cache(baseline_cache_path, role, measured)
    return measured


def run_measurement(
    *,
    patch_path: str,
    role: str,
    config: dict[str, Any] | None,
    clean_source: str,
    baseline_cache_path: str,
) -> dict[str, Any]:
    settings = EvalSettings.from_config(config)
    accuracy_mode = settings.accuracy_mode_for_role(role)
    prefer_docker = (role == "final") and docker_available()
    info: dict[str, Any] = {"role": role, "accuracy_mode": accuracy_mode, "prefer_docker": prefer_docker}

    try:
        baseline = _get_baseline(
            clean_source,
            settings,
            role=role,
            accuracy_mode=accuracy_mode,
            baseline_cache_path=baseline_cache_path,
            prefer_docker=prefer_docker,
        )
    except ServingError as exc:
        return {"ok": False, "gate": f"baseline serving failed: {exc}", "info": info}

    patched_src: Path | None = None
    app_name = f"vllm-serv-opt-patch-{role}-{_short_id()}"
    try:
        patched_src = _apply_patch(clean_source, patch_path)
    except ServingError as exc:
        return {"ok": False, "gate": str(exc), "info": info}

    handle: ServerHandle | None = None
    try:
        try:
            handle = _serve(str(patched_src), settings, app_name=app_name, label=app_name)
        except ServingError as exc:
            return {"ok": False, "gate": f"patched build/serve failed: {exc}", "info": info}

        patched_greedy = collect_greedy_outputs(
            handle.base_url, settings=settings, n=settings.correctness_smoke_prompts
        )
        correctness_ok, mismatches = compare_outputs(
            baseline.get("greedy_outputs", {}), patched_greedy
        )
        info["greedy_mismatches"] = mismatches
        if not correctness_ok:
            return {
                "ok": True,
                "correctness_ok": False,
                "info": info,
                "baseline": {
                    "per_instance_latency": baseline.get("per_instance_latency", {}),
                    "accuracy": baseline.get("accuracy", 0.0),
                },
                "patched": {"per_instance_latency": {}, "accuracy": 0.0},
            }

        results = run_workload(
            base_url=handle.base_url,
            settings=settings,
            role=role,
            prefer_docker=prefer_docker,
        )
        patched_accuracy = compute_accuracy(
            results, settings=settings, mode=accuracy_mode, run_id=f"patched-{role}-{_short_id()}"
        )
        info["baseline_accuracy_mode"] = baseline.get("accuracy_mode")
        info["patched_accuracy_proxy_used"] = bool(patched_accuracy["proxy_used"])
        info["instances"] = len(results)
        return {
            "ok": True,
            "correctness_ok": True,
            "info": info,
            "baseline": {
                "per_instance_latency": baseline.get("per_instance_latency", {}),
                "accuracy": float(baseline.get("accuracy", 0.0)),
            },
            "patched": {
                "per_instance_latency": _latency_map(results),
                "accuracy": float(patched_accuracy["accuracy"]),
            },
        }
    finally:
        if handle is not None:
            stop_server(app_name)
        if patched_src is not None:
            shutil.rmtree(patched_src.parent, ignore_errors=True)


def run_public_test(
    *,
    src: str,
    config: dict[str, Any] | None,
    baseline_cache_path: str,
) -> dict[str, Any]:
    """Agent-facing public test: serve the working tree and report feedback."""
    settings = EvalSettings.from_config(config)
    role = "agent"
    accuracy_mode = settings.accuracy_mode_for_role(role)
    prefer_docker = docker_available()
    app_name = f"vllm-serv-opt-public-{_short_id()}"

    handle: ServerHandle | None = None
    try:
        handle = _serve(src, settings, app_name=app_name, label=app_name)
        measured = _measure_server(
            handle,
            settings,
            role=role,
            accuracy_mode=accuracy_mode,
            prefer_docker=prefer_docker,
            greedy_n=settings.correctness_smoke_prompts,
            run_id=f"public-{_short_id()}",
        )
    except ServingError as exc:
        return {"ok": False, "error": str(exc)}
    finally:
        if handle is not None:
            stop_server(app_name)

    patched_latency = measured["per_instance_latency"]
    result: dict[str, Any] = {
        "ok": True,
        "n_instances": measured["n_instances"],
        "accuracy": measured["accuracy"],
        "accuracy_mode": measured["accuracy_mode"],
        "mean_latency_seconds": (
            sum(patched_latency.values()) / len(patched_latency) if patched_latency else 0.0
        ),
        "per_instance_latency": patched_latency,
    }

    baseline = _load_baseline_cache(baseline_cache_path, role)
    if baseline and baseline.get("per_instance_latency"):
        provisional = provisional_score(
            {str(k): float(v) for k, v in baseline["per_instance_latency"].items()},
            {str(k): float(v) for k, v in patched_latency.items()},
            float(baseline.get("accuracy", 0.0)),
            float(measured["accuracy"]),
            settings.accuracy_tolerance,
        )
        result["baseline_accuracy"] = float(baseline.get("accuracy", 0.0))
        result["provisional"] = provisional
    else:
        result["note"] = "no baseline cache present; reporting raw latency/accuracy only"
    return result
