"""Scoring math shared with the agent-facing public test.

The judge's authoritative scorer lives in the task evaluator; these helpers
mirror it so the public test can show a provisional score consistent with how
the judge will grade. Keep the two in sync.
"""

from __future__ import annotations

import math


def geometric_mean(values: list[float]) -> float:
    if not values:
        return 0.0
    return math.exp(sum(math.log(max(value, 1e-9)) for value in values) / len(values))


def paired_speedups(baseline: dict[str, float], patched: dict[str, float]) -> list[float]:
    speedups: list[float] = []
    for instance_id, patched_value in patched.items():
        base_value = baseline.get(instance_id)
        if base_value is None or patched_value <= 0 or base_value <= 0:
            continue
        speedups.append(max(base_value / patched_value, 0.01))
    return speedups


def score_from_speedup(speedup: float) -> float:
    if speedup <= 0:
        return 0.0
    return max(0.0, min(100.0, 100.0 * math.log(speedup, 2)))


def accuracy_multiplier(baseline_accuracy: float, patched_accuracy: float, tolerance: float) -> float:
    base = max(baseline_accuracy, 1e-9)
    rel_drop = max(0.0, (baseline_accuracy - patched_accuracy) / base)
    if rel_drop <= tolerance:
        return 1.0
    return max(0.0, min(1.0, tolerance / rel_drop))


def provisional_score(
    baseline_latency: dict[str, float],
    patched_latency: dict[str, float],
    baseline_accuracy: float,
    patched_accuracy: float,
    tolerance: float,
) -> dict[str, float]:
    speedups = paired_speedups(baseline_latency, patched_latency)
    gm = geometric_mean(speedups) if speedups else 0.0
    latency_score = score_from_speedup(gm)
    acc_mult = accuracy_multiplier(baseline_accuracy, patched_accuracy, tolerance)
    return {
        "latency_geomean_speedup": gm,
        "latency_score": latency_score,
        "accuracy_multiplier": acc_mult,
        "score": max(0.0, min(100.0, latency_score * acc_mult)),
        "instances_scored": float(len(speedups)),
    }
