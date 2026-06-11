"""Configuration for the vLLM serving evaluation harness.

A single :class:`EvalSettings` is built from the task ``evaluation`` config block
(passed in from the evaluator) with environment-variable fallbacks. The same
settings drive the judge-side measurement and the agent-side public test.
"""

from __future__ import annotations

import os
from dataclasses import dataclass, field
from typing import Any


def _as_int(value: Any, default: int) -> int:
    try:
        return int(value)
    except Exception:
        return default


def _as_float(value: Any, default: float) -> float:
    try:
        return float(value)
    except Exception:
        return default


@dataclass
class EvalSettings:
    model: str = "meta-llama/Llama-3.1-8B-Instruct"
    gpu: str = "L40S"
    dataset: str = "princeton-nlp/SWE-bench_Verified"
    dataset_split: str = "test"
    public_slice: str = "0:5"
    eval_slice: str = "0:30"
    arrival_mode: str = "jps"
    jps: float = 0.5
    workers: int = 8
    step_limit: int = 50
    temperature: float = 0.0
    max_completion_tokens: int = 2048
    accuracy_tolerance: float = 0.05
    agent_accuracy_mode: str = "patch_validity"
    final_accuracy_mode: str = "resolve_rate"
    # Docker Hub namespace for prebuilt SWE-bench eval images (real resolve_rate).
    swebench_namespace: str = "swebench"
    correctness_smoke_prompts: int = 8
    modal_scaledown_seconds: int = 900
    modal_startup_timeout_seconds: int = 1200
    modal_deploy_retries: int = 3
    server_health_timeout_seconds: int = 1800
    build_timeout_seconds: int = 5400
    instance_timeout_seconds: int = 1200
    extra: dict[str, Any] = field(default_factory=dict)

    @classmethod
    def from_config(cls, config: dict[str, Any] | None) -> "EvalSettings":
        config = dict(config or {})
        return cls(
            model=str(config.get("model", cls.model)),
            gpu=str(config.get("gpu", cls.gpu)),
            dataset=str(config.get("dataset", cls.dataset)),
            dataset_split=str(config.get("dataset_split", cls.dataset_split)),
            public_slice=str(config.get("public_slice", cls.public_slice)),
            eval_slice=str(config.get("eval_slice", cls.eval_slice)),
            arrival_mode=str(config.get("arrival_mode", cls.arrival_mode)),
            jps=_as_float(config.get("jps"), cls.jps),
            workers=_as_int(config.get("workers"), cls.workers),
            step_limit=_as_int(config.get("step_limit"), cls.step_limit),
            temperature=_as_float(config.get("temperature"), cls.temperature),
            max_completion_tokens=_as_int(config.get("max_completion_tokens"), cls.max_completion_tokens),
            accuracy_tolerance=_as_float(config.get("accuracy_tolerance"), cls.accuracy_tolerance),
            agent_accuracy_mode=str(config.get("agent_accuracy_mode", cls.agent_accuracy_mode)),
            final_accuracy_mode=str(config.get("final_accuracy_mode", cls.final_accuracy_mode)),
            swebench_namespace=str(config.get("swebench_namespace", cls.swebench_namespace)),
            correctness_smoke_prompts=_as_int(
                config.get("correctness_smoke_prompts"), cls.correctness_smoke_prompts
            ),
            modal_scaledown_seconds=_as_int(config.get("modal_scaledown_seconds"), cls.modal_scaledown_seconds),
            modal_deploy_retries=_as_int(config.get("modal_deploy_retries"), cls.modal_deploy_retries),
            modal_startup_timeout_seconds=_as_int(
                config.get("modal_startup_timeout_seconds"), cls.modal_startup_timeout_seconds
            ),
            server_health_timeout_seconds=_as_int(
                config.get("server_health_timeout_seconds"), cls.server_health_timeout_seconds
            ),
            build_timeout_seconds=_as_int(config.get("build_timeout_seconds"), cls.build_timeout_seconds),
            instance_timeout_seconds=_as_int(config.get("instance_timeout_seconds"), cls.instance_timeout_seconds),
            extra=config,
        )

    def slice_for_role(self, role: str) -> str:
        return self.eval_slice if role == "final" else self.public_slice

    def accuracy_mode_for_role(self, role: str) -> str:
        return self.final_accuracy_mode if role == "final" else self.agent_accuracy_mode


def parse_slice(spec: str, length: int) -> range:
    """Parse a ``start:stop`` slice spec into a concrete index range."""
    spec = (spec or "").strip()
    if not spec:
        return range(length)
    parts = spec.split(":")
    try:
        start = int(parts[0]) if parts[0] else 0
        stop = int(parts[1]) if len(parts) > 1 and parts[1] else length
    except ValueError:
        return range(length)
    start = max(0, min(start, length))
    stop = max(start, min(stop, length))
    return range(start, stop)


def modal_available() -> bool:
    return bool(os.environ.get("MODAL_TOKEN_ID") and os.environ.get("MODAL_TOKEN_SECRET"))
