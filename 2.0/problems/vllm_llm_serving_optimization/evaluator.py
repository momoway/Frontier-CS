"""Evaluator for the experimental vLLM LLM-serving latency optimization task.

The agent submits a Python-only patch against a clean upstream vLLM v0.11.0
checkout. The judge applies the patch, builds and serves the patched vLLM on a
Modal L40S (``meta-llama/Llama-3.1-8B-Instruct``), runs a mini-swe-agent
SWE-bench workload, and scores latency speedup vs a vanilla-vLLM baseline gated
by an accuracy guardrail.

This file contains the self-contained static patch policy and the scoring math.
The heavy orchestration (Modal deploy, workload run, baseline caching) lives in
the ``serving_eval`` package that is baked into the judge image. When that
harness or the serving credentials are not available (for example a local CI
smoke test), the evaluator validates the patch policy and returns a smoke score
so the empty reference patch still passes.
"""

from __future__ import annotations

import fnmatch
import hashlib
import json
import math
import os
import re
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any

MAX_PATCH_BYTES = 1_500_000
MAX_CHANGED_FILES = 80
TASK_CONFIG_PATH = Path("/judge/task_config.json")
SERVING_EVAL_ROOT = "/opt"
DEFAULT_CLEAN_SOURCE = Path("/opt/vllm-clean")


def _load_task_config() -> dict[str, Any]:
    try:
        payload = json.loads(TASK_CONFIG_PATH.read_text(encoding="utf-8"))
    except Exception:
        return {}
    return payload if isinstance(payload, dict) else {}


TASK_CONFIG = _load_task_config()
EVALUATION_CONFIG = (
    TASK_CONFIG.get("evaluation", {}) if isinstance(TASK_CONFIG.get("evaluation"), dict) else {}
)


def _config_value(name: str, default: Any) -> Any:
    return EVALUATION_CONFIG.get(name, default)


def _config_int(name: str, default: int) -> int:
    try:
        return int(EVALUATION_CONFIG.get(name, default))
    except Exception:
        return default


def _config_float(name: str, default: float) -> float:
    try:
        return float(EVALUATION_CONFIG.get(name, default))
    except Exception:
        return default


def _config_str(name: str, default: str) -> str:
    raw = EVALUATION_CONFIG.get(name, default)
    return str(raw)


ACCURACY_TOLERANCE = _config_float("accuracy_tolerance", 0.05)
BASELINE_CACHE_PATH = Path(_config_str("baseline_cache_path", "/opt/vllm-baseline/baseline_metrics.json"))

# --------------------------------------------------------------------------- #
# Patch policy
# --------------------------------------------------------------------------- #

STRONGLY_ALLOWED_PATTERNS = (
    "vllm/v1/core/**",
    "vllm/v1/core/sched/**",
    "vllm/v1/core/kv_cache_utils.py",
    "vllm/config/scheduler.py",
    "vllm/config/cache.py",
)

CONDITIONALLY_ALLOWED_PATTERNS = (
    "vllm/v1/worker/**",
    "vllm/v1/engine/**",
    "vllm/v1/executor/**",
    "vllm/v1/request.py",
    "vllm/v1/outputs.py",
    "vllm/v1/serial_utils.py",
    "vllm/entrypoints/openai/protocol.py",
    "vllm/entrypoints/openai/serving_engine.py",
    "vllm/entrypoints/openai/serving_chat.py",
    "vllm/entrypoints/openai/serving_completion.py",
    "vllm/sampling_params.py",
)

DENIED_PATTERNS = (
    "csrc/**",
    "cmake/**",
    "CMakeLists.txt",
    "setup.py",
    "setup.cfg",
    "pyproject.toml",
    "requirements/**",
    "requirements*.txt",
    "tests/**",
    "benchmarks/**",
    "benchmark/**",
    "docs/**",
    "examples/**",
    "tools/**",
    ".buildkite/**",
    ".github/**",
    "docker/**",
    "Dockerfile*",
    ".dockerignore",
    "vllm/model_executor/models/**",
    "vllm/model_executor/model_loader/**",
    "vllm/transformers_utils/**",
    "vllm/lora/**",
    "vllm/distributed/**",
    "vllm/entrypoints/llm.py",
    "vllm/entrypoints/api_server.py",
    "vllm/entrypoints/cli/**",
    "vllm/version.py",
    "vllm/_version.py",
)

# Source-line tokens that are forbidden in added code: benchmark/dataset names
# (anti hard-coding) and secret/judge environment access (anti exfiltration and
# anti benchmark-detection).
HARD_CODE_TOKENS = (
    "swebench",
    "swe-bench",
    "swe_bench",
    "sweb.eval",
    "minisweagent",
    "mini-swe",
    "princeton-nlp",
    "SWE-bench_Verified",
)

SECRET_TOKENS = (
    "MODAL_TOKEN",
    "MODAL_TOKEN_ID",
    "MODAL_TOKEN_SECRET",
    "HF_TOKEN",
    "HUGGING_FACE_HUB_TOKEN",
    "HUGGINGFACEHUB",
    "FRONTIER_",
    "HARBOR_",
    "JUDGE_URL",
    "RUN_OUTPUT_DIR",
    "scheduler_timestamps",
)

HARD_CODE_TOKEN_RE = re.compile(
    "|".join(re.escape(token) for token in HARD_CODE_TOKENS),
    re.IGNORECASE,
)
SECRET_TOKEN_RE = re.compile("|".join(re.escape(token) for token in SECRET_TOKENS))

# Patches must stay Python-only because the Modal build uses VLLM_USE_PRECOMPILED
# (prebuilt CUDA kernels, Python layer rebuilt). New .py/.pyi files are allowed.
ALLOWED_SOURCE_EXTENSIONS = (".py", ".pyi")


@dataclass(frozen=True)
class PatchFile:
    old_path: str
    new_path: str
    added_lines: tuple[str, ...]
    removed_lines: tuple[str, ...]

    @property
    def path(self) -> str:
        return self.new_path if self.new_path != "/dev/null" else self.old_path


def _match(path: str, patterns: tuple[str, ...]) -> bool:
    return any(fnmatch.fnmatch(path, pattern) for pattern in patterns)


def _is_allowed_source_path(path: str) -> bool:
    return _match(path, STRONGLY_ALLOWED_PATTERNS) or _match(path, CONDITIONALLY_ALLOWED_PATTERNS)


def _invalid(message: str, metrics: dict[str, Any] | None = None):
    payload = metrics or {}
    payload.setdefault("valid_patch", 0)
    return 0.0, 0.0, message, payload


def _parse_patch(text: str) -> list[PatchFile]:
    files: list[PatchFile] = []
    current_old = ""
    current_new = ""
    added: list[str] = []
    removed: list[str] = []
    in_file = False

    for line in text.splitlines():
        if line.startswith("diff --git "):
            if in_file:
                files.append(PatchFile(current_old, current_new, tuple(added), tuple(removed)))
            in_file = True
            current_old = ""
            current_new = ""
            added = []
            removed = []
            continue
        if not in_file:
            continue
        if line.startswith("--- "):
            current_old = line[4:].strip()
            if current_old.startswith("a/"):
                current_old = current_old[2:]
            continue
        if line.startswith("+++ "):
            current_new = line[4:].strip()
            if current_new.startswith("b/"):
                current_new = current_new[2:]
            continue
        if line.startswith("+") and not line.startswith("+++ "):
            added.append(line[1:])
            continue
        if line.startswith("-") and not line.startswith("--- "):
            removed.append(line[1:])

    if in_file:
        files.append(PatchFile(current_old, current_new, tuple(added), tuple(removed)))
    return files


def _validate_patch_path(path: str, metrics: dict[str, Any]) -> tuple[bool, str]:
    if not path or path == "/dev/null":
        return True, ""
    if path.startswith("/") or ".." in Path(path).parts:
        return False, f"unsafe patch path: {path}"
    if _match(path, DENIED_PATTERNS):
        return False, f"changed file is outside task boundary: {path}"
    if not _is_allowed_source_path(path):
        return False, f"changed file is not allowlisted: {path}"
    if not path.endswith(ALLOWED_SOURCE_EXTENSIONS):
        return False, f"only Python source changes are allowed (VLLM_USE_PRECOMPILED build): {path}"
    return True, ""


def validate_patch(patch_path: Path) -> tuple[bool, str, dict[str, Any]]:
    if not patch_path.exists():
        return False, "solution patch does not exist", {}
    size = patch_path.stat().st_size
    if size > MAX_PATCH_BYTES:
        return False, f"patch is too large ({size} bytes > {MAX_PATCH_BYTES})", {}
    text = patch_path.read_text(encoding="utf-8", errors="replace")
    patch_hash = hashlib.sha256(text.encode("utf-8", errors="replace")).hexdigest()
    files = _parse_patch(text)
    metrics: dict[str, Any] = {
        "patch_bytes": size,
        "patch_sha256": patch_hash,
        "changed_files": len(files),
    }
    if len(files) > MAX_CHANGED_FILES:
        return False, f"too many changed files ({len(files)} > {MAX_CHANGED_FILES})", metrics

    for patch_file in files:
        path = patch_file.path
        if patch_file.new_path == "/dev/null":
            return False, f"deleting source files is outside task boundary: {patch_file.old_path}", metrics
        if patch_file.old_path != "/dev/null" and patch_file.old_path != patch_file.new_path:
            ok, error = _validate_patch_path(patch_file.old_path, metrics)
            if not ok:
                return False, f"rename/copy source is outside task boundary: {error}", metrics

        ok, error = _validate_patch_path(path, metrics)
        if not ok:
            return False, error, metrics
        if not path or path == "/dev/null":
            return False, "could not determine changed path from patch", metrics

        added_text = "\n".join(patch_file.added_lines)
        secret_match = SECRET_TOKEN_RE.search(added_text)
        if secret_match:
            return False, f"{path}: judge/secret environment access is forbidden ({secret_match.group(0)})", metrics
        hard_code_match = HARD_CODE_TOKEN_RE.search(added_text)
        if hard_code_match:
            return False, f"{path}: benchmark-specific token is forbidden ({hard_code_match.group(0)})", metrics

    metrics["valid_patch"] = 1
    return True, "patch accepted by static policy", metrics


def patch_is_empty(metrics: dict[str, Any]) -> bool:
    return int(metrics.get("changed_files", 0)) == 0


# --------------------------------------------------------------------------- #
# Scoring
# --------------------------------------------------------------------------- #

def geometric_mean(values: list[float]) -> float:
    if not values:
        return 0.0
    return math.exp(sum(math.log(max(value, 1e-9)) for value in values) / len(values))


def score_from_speedup(speedup: float) -> float:
    if speedup <= 0:
        return 0.0
    raw = 100.0 * math.log(speedup, 2)
    return max(0.0, min(100.0, raw))


def accuracy_multiplier(baseline_accuracy: float, patched_accuracy: float, tolerance: float) -> float:
    base = max(baseline_accuracy, 1e-9)
    rel_drop = max(0.0, (baseline_accuracy - patched_accuracy) / base)
    if rel_drop <= tolerance:
        return 1.0
    return max(0.0, min(1.0, tolerance / rel_drop))


def paired_speedups(
    baseline_latency: dict[str, float],
    patched_latency: dict[str, float],
) -> list[float]:
    speedups: list[float] = []
    for instance_id, patched_value in patched_latency.items():
        base_value = baseline_latency.get(instance_id)
        if base_value is None or patched_value <= 0 or base_value <= 0:
            continue
        speedups.append(max(base_value / patched_value, 0.01))
    return speedups


# --------------------------------------------------------------------------- #
# Error sanitization (black-box safety)
# --------------------------------------------------------------------------- #

def sanitize_error_text(text: str) -> str:
    text = re.sub(r"/tmp/[A-Za-z0-9_./-]+", "<tmp>", text)
    text = re.sub(r"https://[A-Za-z0-9_.:/-]+", "<url>", text)
    for token in SECRET_TOKENS:
        text = text.replace(token, "<redacted>")
    text = re.sub(r"hf_[A-Za-z0-9]+", "<redacted>", text)
    text = re.sub(r"ak-[A-Za-z0-9]+", "<redacted>", text)
    return text[-800:]


# --------------------------------------------------------------------------- #
# Serving harness integration
# --------------------------------------------------------------------------- #

def _serving_harness_available() -> bool:
    if not DEFAULT_CLEAN_SOURCE.exists():
        return False
    if not os.environ.get("MODAL_TOKEN_ID") or not os.environ.get("MODAL_TOKEN_SECRET"):
        return False
    return Path(SERVING_EVAL_ROOT, "serving_eval", "__init__.py").exists()


def _load_serving_eval():
    if SERVING_EVAL_ROOT not in sys.path:
        sys.path.insert(0, SERVING_EVAL_ROOT)
    import serving_eval  # type: ignore

    return serving_eval


def is_final_submission_role() -> bool:
    return os.environ.get("FRONTIER_SUBMISSION_ROLE", "agent") == "final"


def full_evaluation(patch_path: Path, metrics: dict[str, Any]):
    final_role = is_final_submission_role()
    metrics["submission_role"] = "final" if final_role else "agent"

    if not _serving_harness_available():
        # Local smoke / CI: the patch policy passed and the serving stack is not
        # configured here. Return a positive smoke score so the reference patch
        # and policy checks can be validated without a GPU or Modal.
        metrics["full_benchmark"] = 0
        metrics["serving_harness"] = "unconfigured"
        return (
            1.0,
            1.0,
            "patch policy smoke passed; vLLM serving harness is not configured in this environment",
            metrics,
        )

    serving_eval = _load_serving_eval()
    measurement = serving_eval.run_measurement(
        patch_path=str(patch_path),
        role="final" if final_role else "agent",
        config=EVALUATION_CONFIG,
        clean_source=str(DEFAULT_CLEAN_SOURCE),
        baseline_cache_path=str(BASELINE_CACHE_PATH),
    )

    public_info = measurement.get("info", {}) if isinstance(measurement.get("info"), dict) else {}
    for key, value in public_info.items():
        if isinstance(value, (int, float, str, bool)):
            metrics[f"info_{key}"] = value

    if not measurement.get("ok", False):
        gate = str(measurement.get("gate") or "serving evaluation gate failed")
        metrics["gate"] = gate
        return _invalid(gate, metrics)

    if not measurement.get("correctness_ok", False):
        metrics["gate"] = "correctness"
        return _invalid("patched server generations differ from the baseline at temperature 0", metrics)

    patched = measurement.get("patched", {}) or {}
    baseline = measurement.get("baseline", {}) or {}
    patched_latency = {str(k): float(v) for k, v in (patched.get("per_instance_latency") or {}).items()}
    baseline_latency = {str(k): float(v) for k, v in (baseline.get("per_instance_latency") or {}).items()}
    speedups = paired_speedups(baseline_latency, patched_latency)
    if not speedups:
        return _invalid("no paired latency measurements were produced", metrics)

    gm_speedup = geometric_mean(speedups)
    latency_score = score_from_speedup(gm_speedup)

    patched_accuracy = float(patched.get("accuracy", 0.0))
    baseline_accuracy = float(baseline.get("accuracy", 0.0))
    acc_mult = accuracy_multiplier(baseline_accuracy, patched_accuracy, ACCURACY_TOLERANCE)
    bounded = max(0.0, min(100.0, latency_score * acc_mult))

    metrics.update(
        {
            "full_benchmark": 1,
            "serving_harness": "modal_l40s",
            "instances_scored": len(speedups),
            "latency_geomean_speedup": gm_speedup,
            "latency_score": latency_score,
            "baseline_accuracy": baseline_accuracy,
            "patched_accuracy": patched_accuracy,
            "accuracy_multiplier": acc_mult,
        }
    )
    return (
        bounded,
        bounded,
        (
            f"latency geomean speedup {gm_speedup:.4f}x over baseline vLLM; "
            f"accuracy {patched_accuracy:.4f} vs baseline {baseline_accuracy:.4f} "
            f"(multiplier {acc_mult:.3f})"
        ),
        metrics,
    )


def evaluate(solution_path: str) -> tuple[float, float, str, dict[str, Any]]:
    patch_path = Path(solution_path)
    ok, message, metrics = validate_patch(patch_path)
    if not ok:
        return _invalid(message, metrics)
    try:
        return full_evaluation(patch_path, metrics)
    except Exception as exc:  # noqa: BLE001 - black-box: never surface raw internals
        metrics["error_type"] = type(exc).__name__
        metrics["error_detail"] = sanitize_error_text(str(exc))
        return _invalid("serving evaluation failed", metrics)


def prepare() -> dict[str, Any]:
    """Report judge readiness without leaking secret values."""
    return {
        "task": "vllm_llm_serving_optimization",
        "serving_harness_available": _serving_harness_available(),
        "clean_source_present": DEFAULT_CLEAN_SOURCE.exists(),
        "modal_credentials_present": bool(
            os.environ.get("MODAL_TOKEN_ID") and os.environ.get("MODAL_TOKEN_SECRET")
        ),
        "hf_credentials_present": bool(os.environ.get("HF_TOKEN")),
        "baseline_cache_present": BASELINE_CACHE_PATH.exists(),
        "accuracy_tolerance": ACCURACY_TOLERANCE,
    }


def main(argv: list[str]) -> int:
    if len(argv) != 2:
        print("Usage: evaluator.py SOLUTION_PATCH", file=sys.stderr)
        return 2
    score, score_unbounded, message, metrics = evaluate(argv[1])
    print(
        json.dumps(
            {
                "score": score,
                "score_unbounded": score_unbounded,
                "message": message,
                "metrics": metrics,
            },
            indent=2,
            sort_keys=True,
        )
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv))
