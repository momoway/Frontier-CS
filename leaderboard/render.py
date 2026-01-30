#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Render two Frontier-CS leaderboard JSONs (algorithmic.json + research.json) to a
nice GitHub-friendly Markdown block.

Usage:
  python3 render.py \
    --algorithmic algorithmic.json \
    --research research.json \
    --out LEADERBOARD.md

Optional:
  --topk 5
  --sort elo            # one of: elo, score_at_1, avg_at_5, score_at_5
  --no-bold-best        # disable bolding best-per-column
  --readme README.md --block FRONTIERCS_LEADERBOARD   # inject into README block
"""

from __future__ import annotations

import argparse
import json
import math
from pathlib import Path
from typing import Any, Dict, List, Optional, Tuple

NUM_KEYS = ["score_at_1", "avg_at_5", "score_at_5", "elo"]


def load_json(p: Path) -> Dict[str, Any]:
    if not p.exists():
        raise FileNotFoundError(f"Missing file: {p.resolve()}")
    with p.open("r", encoding="utf-8") as f:
        return json.load(f)


def escape_md(s: str) -> str:
    return s.replace("|", "\\|")


def fmt_float(x: Any, decimals: int = 2) -> str:
    if x is None:
        return "–"
    if isinstance(x, (int, float)) and not isinstance(x, bool):
        if isinstance(x, float) and math.isnan(x):
            return "–"
        return f"{float(x):.{decimals}f}"
    return str(x)


def fmt_elo(x: Any) -> str:
    if x is None:
        return "–"
    if isinstance(x, (int, float)) and not isinstance(x, bool):
        if isinstance(x, float) and math.isnan(x):
            return "–"
        return str(int(round(float(x))))
    return str(x)


def sort_entries(entries: List[Dict[str, Any]], key: str, desc: bool = True) -> List[Dict[str, Any]]:
    def kfn(e: Dict[str, Any]) -> Tuple[int, float]:
        v = e.get(key, None)
        missing = 1 if v is None else 0
        vv = float(v) if v is not None else (float("-inf") if desc else float("inf"))
        return (missing, vv)

    return sorted(entries, key=kfn, reverse=desc)


def best_by_column(entries: List[Dict[str, Any]], keys: List[str]) -> Dict[str, Any]:
    best: Dict[str, Any] = {}
    for k in keys:
        vals = [e.get(k, None) for e in entries if e.get(k, None) is not None]
        best[k] = max(vals) if vals else None
    return best


def maybe_bold(s: str, cond: bool) -> str:
    return f"**{s}**" if cond else s


def medal(rank: int) -> str:
    return "🥇" if rank == 1 else ("🥈" if rank == 2 else ("🥉" if rank == 3 else str(rank)))


def render_track(
    title: str,
    anchor: str,
    entries: List[Dict[str, Any]],
    sort_key: str,
    topk: Optional[int],
    bold_best: bool,
    show_human_ref: bool = True,
) -> str:
    humans = [e for e in entries if e.get("is_human", False)]
    models = [e for e in entries if not e.get("is_human", False)]

    ranked = sort_entries(models, sort_key, desc=True)
    if topk is not None:
        ranked = ranked[:topk]

    best = best_by_column(ranked, ["score_at_1", "avg_at_5", "score_at_5", "elo"]) if bold_best else {}

    lines: List[str] = []
    lines.append(f'<a id="{anchor}"></a>')
    lines.append(f"### {title}")
    lines.append("")
    lines.append("| Rank | Model | Score@1 | Avg@5 | Score@5 | Elo |")
    lines.append("|:---:|---|---:|---:|---:|---:|")

    for i, e in enumerate(ranked, start=1):
        s1_raw = e.get("score_at_1", None)
        a5_raw = e.get("avg_at_5", None)
        s5_raw = e.get("score_at_5", None)
        elo_raw = e.get("elo", None)

        s1 = fmt_float(s1_raw, 2)
        a5 = fmt_float(a5_raw, 2)
        s5 = fmt_float(s5_raw, 2)
        elo = fmt_elo(elo_raw)

        if bold_best and best:
            s1 = maybe_bold(s1, s1_raw is not None and s1_raw == best.get("score_at_1"))
            a5 = maybe_bold(a5, a5_raw is not None and a5_raw == best.get("avg_at_5"))
            s5 = maybe_bold(s5, s5_raw is not None and s5_raw == best.get("score_at_5"))
            elo = maybe_bold(elo, elo_raw is not None and elo_raw == best.get("elo"))

        lines.append(f"| {medal(i)} | {escape_md(e['model'])} | {s1} | {a5} | {s5} | {elo} |")

    lines.append("")
    if show_human_ref and humans:
        # Show a compact human reference line (Score@1 only is often the most meaningful)
        # If you prefer a full row, tweak this.
        h = humans[0]
        hs1 = fmt_float(h.get("score_at_1", None), 2)
        lines.append(f"<sub>Human reference: <b>{hs1}</b> (Score@1).</sub>")
        lines.append("")
    return "\n".join(lines)


def render_markdown(
    algo_entries: List[Dict[str, Any]],
    research_entries: List[Dict[str, Any]],
    topk: Optional[int],
    sort_key: str,
    bold_best: bool,
) -> str:
    parts: List[str] = []
    parts.append("## 🏆 Leaderboard Snapshot\n")
    parts.append(
        "<sub>"
        "Score@k = best-of-k runs; Avg@k = average over k runs; "
        "Elo uses Bradley–Terry from single-attempt performance (difficulty-normalized)."
        "</sub>\n"
    )
    parts.append("")
    parts.append(render_track("Algorithmic Track (172 problems)", "algorithmic-track", algo_entries, sort_key, topk, bold_best))
    parts.append(render_track("Research Track (68 problems)", "research-track", research_entries, sort_key, topk, bold_best))
    parts.append("➡️ **[View full leaderboard →](https://frontier-cs.org/leaderboard)**\n")
    return "\n".join(parts).strip() + "\n"


def inject_block(readme_path: Path, block_name: str, new_md: str) -> None:
    start = f"<!-- BEGIN {block_name} -->"
    end = f"<!-- END {block_name} -->"
    text = readme_path.read_text(encoding="utf-8")
    if start not in text or end not in text:
        raise RuntimeError(f"Block markers not found in {readme_path}: {start} / {end}")

    pre = text.split(start)[0]
    post = text.split(end)[1]
    injected = pre + start + "\n\n" + new_md.strip() + "\n\n" + end + post
    readme_path.write_text(injected, encoding="utf-8")


def main() -> None:
    ap = argparse.ArgumentParser()
    ap.add_argument("--algorithmic", type=Path, default=Path("algorithmic.json"))
    ap.add_argument("--research", type=Path, default=Path("research.json"))
    ap.add_argument("--out", type=Path, default=Path("LEADERBOARD.md"))
    ap.add_argument("--topk", type=int, default=5, help="Top-K rows per track (ranked models). Use 0 to show all.")
    ap.add_argument("--sort", type=str, default="elo", choices=["elo", "score_at_1", "avg_at_5", "score_at_5"])
    ap.add_argument("--no-bold-best", action="store_true")
    ap.add_argument("--readme", type=Path, default=None, help="If set, inject markdown into README block.")
    ap.add_argument("--block", type=str, default="FRONTIERCS_LEADERBOARD")
    args = ap.parse_args()

    algo = load_json(args.algorithmic)
    research = load_json(args.research)

    algo_entries = algo.get("entries", [])
    research_entries = research.get("entries", [])

    topk = None if args.topk == 0 else args.topk
    md = render_markdown(
        algo_entries=algo_entries,
        research_entries=research_entries,
        topk=topk,
        sort_key=args.sort,
        bold_best=not args.no_bold_best,
    )

    args.out.write_text(md, encoding="utf-8")
    print(f"Wrote Markdown: {args.out.resolve()}")

    if args.readme is not None:
        inject_block(args.readme, args.block, md)
        print(f"Injected into: {args.readme.resolve()}  (block: {args.block})")


if __name__ == "__main__":
    main()
