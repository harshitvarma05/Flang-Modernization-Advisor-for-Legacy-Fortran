from __future__ import annotations

import argparse
from pathlib import Path

from .analyzer import ModernizationAnalyzer
from .reporter import to_json, to_markdown
from .transform import apply_safe_transformations


def main() -> int:
    parser = argparse.ArgumentParser(description="Flang Modernization Advisor for legacy Fortran")
    parser.add_argument("path", type=Path, help="Fortran file or directory to analyze")
    parser.add_argument("--format", choices=["markdown", "json"], default="markdown")
    parser.add_argument("--output", type=Path, help="Write report to this file")
    parser.add_argument("--safe-transform-out", type=Path, help="Write conservative transformed copies to this directory")
    args = parser.parse_args()

    analysis = ModernizationAnalyzer().analyze_path(args.path)
    report = to_json(analysis) if args.format == "json" else to_markdown(analysis)
    if args.output:
        args.output.parent.mkdir(parents=True, exist_ok=True)
        args.output.write_text(report)
    else:
        print(report)

    if args.safe_transform_out:
        changed = apply_safe_transformations(args.path, args.safe_transform_out)
        print(f"\nSafe transformed files written: {len(changed)}")
        for path in changed:
            print(f"- {path}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
