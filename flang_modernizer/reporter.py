from __future__ import annotations

import json
from collections import Counter

from .models import ProjectAnalysis


def to_json(analysis: ProjectAnalysis) -> str:
    return json.dumps(
        {
            "root": str(analysis.root),
            "files_analyzed": len(analysis.files),
            "findings": [finding.as_dict() for finding in analysis.sorted_findings()],
        },
        indent=2,
    )


def to_markdown(analysis: ProjectAnalysis) -> str:
    counts = Counter(f.pattern for f in analysis.findings)
    lines = [
        "# Flang Modernization Advisor Report",
        "",
        f"Analyzed root: `{analysis.root}`",
        f"Files analyzed: **{len(analysis.files)}**",
        f"Findings: **{len(analysis.findings)}**",
        "",
        "## Pattern Summary",
        "",
        "| Pattern | Count |",
        "|---|---:|",
    ]
    for pattern, count in sorted(counts.items()):
        lines.append(f"| {pattern} | {count} |")

    lines.extend(
        [
            "",
            "## Prioritized Modernization Plan",
            "",
            "| Priority | Pattern | Location | Effort | Safety | Recommendation |",
            "|---:|---|---|---|---|---|",
        ]
    )
    for finding in analysis.sorted_findings():
        recommendation = _recommendation(finding.pattern)
        lines.append(
            f"| {finding.priority} | {finding.pattern} | `{finding.location.display()}` | "
            f"{finding.effort.value} | {finding.safety.value} | {recommendation} |"
        )

    lines.extend(["", "## Impact Details", ""])
    for finding in analysis.sorted_findings():
        lines.append(f"### {finding.pattern} at `{finding.location.display()}`")
        lines.append(f"- Routine: `{finding.routine or 'file scope'}`")
        lines.append(f"- Affected files: {len(finding.affected_files)}")
        lines.append(f"- Dependent constructs: {', '.join(finding.dependent_constructs) or 'none recorded'}")
        lines.append(f"- Behavior risks: {', '.join(finding.behavior_risks) or 'low'}")
        lines.append("")
    return "\n".join(lines)


def _recommendation(pattern: str) -> str:
    return {
        "arithmetic-if": "Replace with explicit IF/ELSE IF/ELSE branches after checking label fall-through.",
        "computed-goto": "Replace branch table with SELECT CASE or procedure dispatch.",
        "equivalence": "Isolate storage overlay and replace only after aliasing/binary-layout review.",
        "common-block": "Migrate shared state to a module, preserving declaration order and initialization.",
        "implicit-typing": "Add IMPLICIT NONE and explicit declarations per program unit.",
        "statement-function": "Convert to an internal or module procedure.",
        "fixed-form": "Convert to free-form source and normalize continuations/comments.",
        "assumed-size-array": "Use assumed-shape arrays after introducing explicit interfaces.",
        "entry": "Split alternate entries into separate procedures with explicit shared state.",
    }.get(pattern, "Review construct and modernize locally.")
