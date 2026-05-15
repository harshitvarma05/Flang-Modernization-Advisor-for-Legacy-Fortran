from __future__ import annotations

import re
from pathlib import Path
from typing import List


DECLARATION_RE = re.compile(r"^\s*(integer|real|double\s+precision|complex|logical|character)\b", re.IGNORECASE)
STATEMENT_FUNCTION_RE = re.compile(r"^\s*([a-z_]\w*)\s*\(([^)]*)\)\s*=\s*(.+)$", re.IGNORECASE)


def apply_safe_transformations(source: Path, output_dir: Path) -> List[Path]:
    """Apply deliberately conservative demo transformations.

    These transformations are syntax-preserving cleanups intended to validate the
    advisor's low-risk recommendations. They do not attempt semantic rewrites for
    COMMON, EQUIVALENCE, ENTRY, or computed branches.
    """

    output_dir.mkdir(parents=True, exist_ok=True)
    changed: List[Path] = []
    paths = [source] if source.is_file() else sorted(p for p in source.rglob("*") if p.suffix.lower() in {".f", ".for", ".f77"})
    for path in paths:
        original = path.read_text(errors="replace").splitlines()
        rewritten = [_rewrite_fixed_line(line) for line in original]
        rewritten = _convert_simple_statement_functions(rewritten)
        target = output_dir / path.with_suffix(".f90").name
        target.write_text("\n".join(rewritten) + "\n")
        changed.append(target)
    return changed


def _rewrite_fixed_line(line: str) -> str:
    if not line:
        return line
    if line[:1].upper() in {"C", "*"}:
        return "!" + line[1:]
    if len(line) >= 6 and line[:5].strip().isdigit():
        return line[:5].strip() + " " + line[6:].rstrip()
    if len(line) >= 6 and line[5].strip():
        return "& " + line[6:].rstrip()
    return line.rstrip()


def _convert_simple_statement_functions(lines: List[str]) -> List[str]:
    result: List[str] = []
    declared: set[str] = set()
    pending_functions: List[str] = []
    in_routine = False

    for line in lines:
        stripped = line.strip()
        if re.match(r"^(program|subroutine|function)\b", stripped, re.IGNORECASE):
            in_routine = True
            declared = set()
            pending_functions = []

        if in_routine and DECLARATION_RE.match(stripped):
            declared.update(_declared_names(stripped))

        stmt_fn = STATEMENT_FUNCTION_RE.match(stripped) if in_routine else None
        if stmt_fn and stmt_fn.group(1).lower() not in declared:
            pending_functions.extend(_internal_function_lines(stmt_fn.group(1), stmt_fn.group(2), stmt_fn.group(3)))
            continue

        if in_routine and re.match(r"^end\b", stripped, re.IGNORECASE):
            if pending_functions:
                result.append("contains")
                result.extend(pending_functions)
            in_routine = False

        result.append(line)
    return result


def _declared_names(code: str) -> set[str]:
    _, _, tail = code.partition("::")
    if not tail:
        tail = re.sub(
            r"^\s*(integer|real|complex|logical|character|double\s+precision)"
            r"(?:\s*\*?\s*\d+|\s*\([^)]*\))?\s*",
            "",
            code,
            flags=re.IGNORECASE,
        )
    names = set()
    for part in tail.split(","):
        name = re.sub(r"\(.*", "", part).strip().lower()
        if re.match(r"^[a-z_]\w*$", name):
            names.add(name)
    return names


def _internal_function_lines(name: str, args: str, expression: str) -> List[str]:
    arg_names = [arg.strip() for arg in args.split(",") if arg.strip()]
    lines = [f"  real function {name}({', '.join(arg_names)})"]
    for arg in arg_names:
        lines.append(f"    real, intent(in) :: {arg}")
    lines.append(f"    {name} = {expression.strip()}")
    lines.append(f"  end function {name}")
    return lines
