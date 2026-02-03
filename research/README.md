# Research Problems

> For complete model evaluation workflow (prepare solutions, run batch evaluation, submit to leaderboard), see [SUBMIT.md](../SUBMIT.md).

Real-world systems challenges requiring domain expertise in GPU computing, distributed systems, ML pipelines, databases, and security.

## Basic Usage

```bash
# List all problems
frontier list research

# Evaluate a solution locally (uses Docker by default)
frontier eval research flash_attn <your_solution.py>

# Evaluate on cloud with SkyPilot
frontier eval research flash_attn <your_solution.py> --skypilot

# Evaluate multiple problems
frontier eval research --problems flash_attn,cross_entropy <your_solution.py>
```

## SkyPilot Setup

Some problems require GPUs or specific hardware. Use [SkyPilot](https://skypilot.readthedocs.io/) to run evaluations on cloud VMs.

```bash
sky check
```

See [SkyPilot docs](https://skypilot.readthedocs.io/en/latest/getting-started/installation.html) for cloud credential setup.

## Batch Evaluation

Batch evaluation automatically scans `solutions/` and parses problem IDs from filenames:

```bash
# Evaluate all solutions (uses SkyPilot by default, auto-skips completed)
frontier-eval batch research

# With custom parallelism
frontier-eval batch research --workers 20 --clusters 4

# Check status
frontier-eval batch research --status

# Force re-evaluate all
frontier-eval batch research --no-resume

# Retry failed evaluations
frontier-eval batch research --retry-failed
```

**Parameters:**
- `--workers`: Number of parallel workers (default: 10)
- `--clusters`: Number of SkyPilot clusters for load-balancing (default: same as workers, research + skypilot only)

With `--workers 20 --clusters 4`, 20 workers share 4 clusters via load-balancing.

## Python API

```python
from frontier_cs import FrontierCSEvaluator

evaluator = FrontierCSEvaluator()

# Single problem
result = evaluator.evaluate("research", problem_id="flash_attn", code=my_code)
print(f"Score: {result.score}")

# With SkyPilot
result = evaluator.evaluate("research", problem_id="flash_attn", code=my_code,
                           backend="skypilot")
```

## Problem Structure

Each problem is in its own directory under `research/problems/`:

```
research/problems/
├── flash_attn/           # Single problem
│   ├── config.yaml
│   ├── readme
│   ├── evaluator.py
│   └── resources/
├── gemm_optimization/    # Problem with variants
│   ├── squares/
│   ├── rectangles/
│   └── ...
└── ...
```

### File Reference

| File                   | Purpose                                                 |
| ---------------------- | ------------------------------------------------------- |
| `config.yaml`          | Runtime config (Docker image, GPU, timeout, dependencies) |
| `readme`               | Problem description, API spec, scoring formula          |
| `set_up_env.sh`        | Dataset preparation only (deps handled by framework)    |
| `evaluate.sh`          | Evaluation entry point                                  |
| `evaluator.py`         | Core evaluation logic                                   |
| `resources/`           | Baseline code, benchmark, test data, pyproject.toml     |

**Note:** `resources/`, `common/`, and `__pycache__/` directories are excluded from problem detection. A valid problem directory must contain `evaluator.py` or `evaluate.py`.

> For creating new problems (config.yaml format, evaluation scripts, uv_overrides.txt), see [CONTRIBUTING.md](../CONTRIBUTING.md#research-problems).

## Solution Interface

Submit a `solution.py` implementing the `Solution` class. The interface varies by problem type:

### Triton Kernel Problems (flash_attn, cross_entropy, gemm_optimization...)

```python
class Solution:
    def solve(self, spec_path: str = None) -> dict:
        """
        Returns either:
        - {"code": "python_code_string"}
        - {"program_path": "path/to/kernel.py"}
        """
        kernel_code = '''
import triton
import triton.language as tl

@triton.jit
def my_kernel(...):
    ...

def entry_function(...):
    ...
'''
        return {"code": kernel_code}
```

### ML Training Problems (imagenet_pareto...)

```python
class Solution:
    def solve(self, train_loader, val_loader, metadata: dict) -> torch.nn.Module:
        """
        Train and return a model.

        metadata contains: num_classes, input_dim, param_limit,
                          baseline_accuracy, device, etc.
        """
        model = MyModel(...)
        # training loop
        return model
```

### Other Problems

Check each problem's `readme` for the specific `solve()` signature and return type.

