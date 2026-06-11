#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)
TASK_DIR=$(cd "$SCRIPT_DIR/.." && pwd)

VLLM_REF="${VLLM_REF:-v0.11.0}"
AGENT_TAG="${AGENT_TAG:-frontiercs/vllm-serving-optimization-agent:experimental-v0.11.0}"
JUDGE_TAG="${JUDGE_TAG:-frontiercs/vllm-serving-optimization-judge:experimental-v0.11.0}"

# Build context is the task directory so the Dockerfiles can COPY serving_eval.
docker build \
  --build-arg "VLLM_REF=$VLLM_REF" \
  -f "$TASK_DIR/docker/agent/Dockerfile" \
  -t "$AGENT_TAG" \
  "$TASK_DIR"

docker build \
  --build-arg "VLLM_REF=$VLLM_REF" \
  -f "$TASK_DIR/docker/judge/Dockerfile" \
  -t "$JUDGE_TAG" \
  "$TASK_DIR"

echo "Built:"
echo "  $AGENT_TAG"
echo "  $JUDGE_TAG"
