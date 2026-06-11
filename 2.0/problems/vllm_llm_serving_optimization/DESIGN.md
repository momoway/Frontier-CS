# vLLM LLM-Serving Optimization — Design & Operations

A Frontier-CS **2.0** systems task. The agent patches a **clean upstream vLLM
v0.11.0** checkout (Python-only) to reduce the **end-to-end latency** of an LLM
serving system on a multi-turn agentic workload, while keeping task-solving
**accuracy** close to a vanilla-vLLM baseline. The served model is
`meta-llama/Llama-3.1-8B-Instruct` on a single **NVIDIA L40S** provisioned
on-demand through [Modal](https://modal.com/docs).

> **Validated end-to-end (2026-06-11):** a full Harbor trial with the `codex`
> agent (`gpt-5.5`) produced a real **1.79× latency geomean speedup** over the
> baseline at full eval scale (30 SWE-bench instances), accuracy preserved →
> **score 83.89 / 100**.

---

## 1. Current Setting

All knobs live in `config.yaml` (`evaluation` block) and are baked into the
judge/agent images as `task_config.json`.

| Parameter | Value | Notes |
|---|---|---|
| Served model | `meta-llama/Llama-3.1-8B-Instruct` | gated; HF token required |
| Serving GPU | **1× NVIDIA L40S** (via Modal) | one GPU per environment |
| Workload | mini-swe-agent on `princeton-nlp/SWE-bench_Verified` (split `test`) | multi-turn, shared-prefix conversations |
| Arrival | Poisson, `jps = 0.5` jobs/s | concurrent in-flight conversations |
| `public_slice` (agent role) | `0:5` | iterative self-test subset |
| `eval_slice` (final role) | `0:30` | full verification; superset of public |
| Decoding | `temperature = 0`, `max_completion_tokens = 2048` | greedy, deterministic |
| `step_limit` | 50 | per-instance agent steps |
| Accuracy (agent role) | `patch_validity` | cheap proxy for iterative feedback |
| Accuracy (final role) | `resolve_rate` | SWE-bench resolve; falls back to `patch_validity` if the judge has no Docker-in-Docker |
| `accuracy_tolerance` | `0.05` | ≤5% relative drop ⇒ no penalty |
| `correctness_smoke_prompts` | 8 | greedy outputs must match baseline token-for-token |
| Build timeout / per-instance timeout | 5400 s / 1200 s | |
| Submission | file `/app/solution.patch` (git diff vs `/app/vllm`), `max_queue_size = 2` | async |
| Container budget | 8 vCPU, 32 GiB RAM, 64 GiB storage | agent **and** judge; GPU is remote on Modal |

**Two roles, two scales.** *Agent role* (iterative `submit.sh` / `public_test`)
uses `public_slice` + `patch_validity`; *final role* (the Harbor verifier) uses
`eval_slice` + `resolve_rate`. The public subset is a strict subset of the final
set, so the self-test is a fast, faithful proxy.

---

## 2. Scoring

The judge serves **baseline (vanilla vLLM)** and the **patched build** on the
same L40S, under the same workload and the same arrival schedule, and measures
per-instance end-to-end latency (arrival of an instance's first request →
completion of its last response), client-side.

**Hard gates → score 0** (checked before any timing):
1. **Patch policy** (see §3) — disallowed file, non-Python, secret access, or
   benchmark hard-coding.
2. **Build** — the patched source must build on Modal (`VLLM_USE_PRECOMPILED`).
3. **Server health** — `/v1/models` must come up.
4. **Correctness** — the patched server's greedy outputs must match the baseline
   **token-for-token** at `temperature 0` on a small smoke set. An optimization
   must not change what the model generates.

**Latency score** (primary objective — geometric mean of per-instance speedups):
```
per_instance_speedup[i] = baseline_latency[i] / patched_latency[i]   # floored at 0.01
latency_speedup         = geomean(per_instance_speedup)
latency_score           = clip(100 * log2(latency_speedup), 0, 100)
```
`1.0×` → 0 points, `2.0×` → 100 points, regressions → 0. Geomean rewards broad
speedups over a single large outlier.

**Accuracy guardrail** (multiplier):
```
rel_drop = max(0, (baseline_accuracy - patched_accuracy) / baseline_accuracy)
acc_mult = 1.0                       if rel_drop <= 0.05      # within 5% → no penalty
acc_mult = clip(0.05 / rel_drop, 0, 1)  otherwise            # inverse-proportional decay
```

**Final score**:
```
score  = clip(latency_score * acc_mult, 0, 100)
reward = score / 100        # Harbor reward.txt
```
A fast build that degrades task quality loses most of its score; a build within
5% of baseline accuracy is scored purely on its latency improvement.

Authoritative scorer: `evaluator.py` (`full_evaluation`); `serving_eval/scoring.py`
mirrors it for the agent-side public test's provisional score. When the serving
stack is unconfigured (no Modal/clean source, e.g. local CI), the evaluator
returns a `1.0` smoke score so the empty reference patch passes.

---

## 3. Which vLLM files the model may change (Patch Policy)

The patch is validated **before** building. Build uses `VLLM_USE_PRECOMPILED=1`,
so **only Python source is allowed** (`.py`, `.pyi`); no CUDA/C++, build-system,
packaging, or dependency changes. New Python files inside allowed areas are OK.

**Strongly allowed** (core scheduling / batching / KV-cache):
```
vllm/v1/core/**
vllm/v1/core/sched/**
vllm/v1/core/kv_cache_utils.py
vllm/config/scheduler.py
vllm/config/cache.py
```

**Conditionally allowed** (narrow wiring around the engine / request path):
```
vllm/v1/worker/**          vllm/v1/engine/**         vllm/v1/executor/**
vllm/v1/request.py         vllm/v1/outputs.py        vllm/v1/serial_utils.py
vllm/entrypoints/openai/protocol.py
vllm/entrypoints/openai/serving_engine.py
vllm/entrypoints/openai/serving_chat.py
vllm/entrypoints/openai/serving_completion.py
vllm/sampling_params.py
```

**Denied** (rejected outright):
```
csrc/** cmake/** CMakeLists.txt setup.py setup.cfg pyproject.toml
requirements/** requirements*.txt
tests/** benchmarks/** docs/** examples/** tools/** .github/** docker/** Dockerfile*
vllm/model_executor/models/**     vllm/model_executor/model_loader/**
vllm/transformers_utils/**  vllm/lora/**  vllm/distributed/**
vllm/entrypoints/llm.py  vllm/entrypoints/api_server.py  vllm/entrypoints/cli/**
vllm/version.py  vllm/_version.py
```

**Also rejected:** reading/writing judge/Modal/HF/Frontier/Harbor environment
variables (`MODAL_TOKEN*`, `HF_TOKEN`, `FRONTIER_*`, `HARBOR_*`, `JUDGE_URL`,
`RUN_OUTPUT_DIR`, scheduler-timestamp leakage), and hard-coding the benchmark /
dataset / instance ids / judge paths (`swebench`, `princeton-nlp`,
`SWE-bench_Verified`, `minisweagent`, …). The server is launched under a fixed
config; patches that detect the benchmark, sleep, short-circuit generation, or
otherwise special-case the evaluation are rejected.

> **In practice:** the intended optimization area is *online serving efficiency*
> — request scheduling, batching, KV-cache management, prefix/prompt-cache reuse,
> preemption/admission control, queueing, and closely related scheduler/execution
> wiring. The validated 1.79× run was a single-file change to
> `vllm/v1/core/sched/scheduler.py`. (Candidate variants during the run also
> touched `vllm/v1/core/kv_cache_utils.py`, `vllm/v1/core/kv_cache_manager.py`,
> and `vllm/config/scheduler.py` — all within the allowlist.)

---

## 4. GPU resource management & scheduling (Modal)

**No local GPU.** The agent and judge containers are CPU-only clients
(8 vCPU / 32 GiB). The single L40S is provisioned **on-demand on Modal** and is
the *only* place the model runs. This is what makes the agent/judge split cheap
to host.

### Image build (per submission)
`serving_eval/modal_app.py` defines a Modal app parametrized entirely via env
vars (so the same module serves baseline and patched trees):
- Base `nvidia/cuda:12.9.0-devel-ubuntu22.04` (+ Python 3.12, `uv`).
- `add_local_dir(<vllm_src>, /src/vllm, copy=True)` bakes the **target source tree**
  into the image (`copy=True` is required because the next step installs from it).
- `VLLM_USE_PRECOMPILED=1 uv pip install --system -e .` — reuses vLLM's prebuilt
  CUDA kernels and rebuilds only the Python layer ⇒ per-submission builds are
  minutes, not an hour, and the **Python-only patch policy is enforced by
  construction**.
- Pinned for reproducibility on a shallow/patched tree:
  `SETUPTOOLS_SCM_PRETEND_VERSION*` (version detection), a pinned
  `VLLM_PRECOMPILED_WHEEL_LOCATION` (ABI-matched release wheel — the default
  derivation falls back to an incompatible nightly), `transformers==4.55.2`
  (the unpinned upper bound otherwise resolves to an incompatible 5.x), and
  `hf_transfer`.

### Serving
```python
@app.function(gpu="L40S", scaledown_window=900, secrets=[huggingface-secret],
              volumes={hf_cache, vllm_cache})
@modal.concurrent(max_inputs=64)
@modal.web_server(port=8000, startup_timeout=...)
def serve(): subprocess.Popen("vllm serve <model> --host 0.0.0.0 --port 8000 ...")
```
- `gpu="L40S"` requests exactly one L40S; `@modal.concurrent(64)` lets one
  warm container handle many in-flight requests (matching the Poisson workload).
- `@modal.web_server` exposes vLLM's OpenAI endpoint at a stable
  `https://…modal.run/v1`; Modal cold-starts the container on first request and
  serves within `startup_timeout`.
- **Persisted caches:** a `huggingface` Volume (weights downloaded once, reused
  across cold starts) and a `vllm` cache Volume.
- `scaledown_window=900` releases the idle GPU after 15 min — you pay for GPU
  only while serving/measuring.

### Lifecycle & scheduling (`serving_eval/serving.py`)
```
deploy_server() → `modal deploy modal_app.py` (env selects src/model/app-name)
               → Function.from_name(app, "serve").get_web_url()
wait_healthy() → poll /v1/models until 200
... run workload ...
stop_server()  → `modal app stop <app>`
```
- **One L40S per environment is honored by serializing:** baseline and patched
  are **never served concurrently**. The baseline is measured once and cached
  (`/opt/vllm-baseline/baseline_metrics.json`); the patched build is then served
  on its own and its greedy outputs are compared against the cached baseline.
- **Transient-failure retry:** Modal occasionally evicts an image build under
  load (`Image build terminated due to external shut-down`, `APP_STATE_STOPPED`,
  gateway timeouts). `deploy_server` retries such transient deploys with backoff
  (`deploy_retries`, default 3), running `modal app stop` between attempts; a
  genuine build error in the patch is non-transient and fails fast.
- Auth inside the containers is env-var based (`MODAL_TOKEN_ID` /
  `MODAL_TOKEN_SECRET`); gated Llama weights are pulled inside the Modal serving
  container via the Modal Secret `huggingface-secret` (key `HF_TOKEN`).

### Where Modal is used from
Both the **agent's async public test** (`harbor/app/public_test.py` →
`serving_eval.run_public_test`) and the **judge's measurement**
(`evaluator.py` → `serving_eval.run_measurement`) drive Modal the same way, so
the iterative feedback the agent sees is the same kind the judge grades on.

---

## File map

```
config.yaml          resources, model, L40S, dataset, eval knobs (→ task_config.json)
readme               public problem statement (no algorithm hints)
evaluator.py         patch policy + scoring + orchestration (+ local smoke degrade)
serving_eval/        settings · modal_app · serving · sandbox · agent_runner ·
                     accuracy · correctness · scoring · measure
docker/              agent + judge Dockerfiles, build/smoke scripts
harbor/app/          make_submission.sh, public_test client
```
