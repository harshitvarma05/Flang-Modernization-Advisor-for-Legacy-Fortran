from __future__ import annotations

import re
from collections import defaultdict
from pathlib import Path
from typing import Iterable, List

from .models import Effort, Finding, Location, ProjectAnalysis, Routine, Safety, SourceUnit


FORTRAN_EXTENSIONS = {".f", ".for", ".ftn", ".f77", ".f90", ".f95", ".f03", ".f08"}
DECLARATION_RE = re.compile(
    r"^\s*(integer|real|double\s+precision|complex|logical|character|type\s*\(|class\s*\()",
    re.IGNORECASE,
)
ROUTINE_RE = re.compile(
    r"^\s*(program|subroutine|function|block\s+data)\s+([a-z_]\w*)?",
    re.IGNORECASE,
)
END_ROUTINE_RE = re.compile(r"^\s*end\s*(program|subroutine|function|block\s+data)?\b", re.IGNORECASE)
COMMON_RE = re.compile(r"\bcommon\s*(?:/\s*([a-z_]\w*)\s*/)?\s*(.*)", re.IGNORECASE)
EQUIVALENCE_RE = re.compile(r"\bequivalence\s*\((.*)\)", re.IGNORECASE)
ASSUMED_SIZE_RE = re.compile(r"\b[a-z_]\w*\s*\([^)]*\*[^)]*\)", re.IGNORECASE)
STATEMENT_FUNCTION_RE = re.compile(r"^\s*([a-z_]\w*)\s*\(([^)]*)\)\s*=", re.IGNORECASE)
ENTRY_RE = re.compile(r"^\s*entry\s+([a-z_]\w*)", re.IGNORECASE)


class ModernizationAnalyzer:
    """Static analyzer for legacy Fortran modernization planning.

    The detector operates over normalized source lines in this lab version. In a
    full Flang integration, SourceUnit and Routine facts would be populated from
    Flang's parse tree and semantic symbols instead of the regex front-end below.
    The impact and prioritization stages are intentionally independent of that
    front-end boundary.
    """

    def analyze_path(self, path: Path) -> ProjectAnalysis:
        root = path.resolve()
        files = list(self._load_sources(root))
        for unit in files:
            self._index_routines(unit)

        findings: List[Finding] = []
        for unit in files:
            findings.extend(self._detect_in_unit(unit))

        self._compute_impact(files, findings)
        self._prioritize(findings)
        return ProjectAnalysis(root=root, files=files, findings=findings)

    def _load_sources(self, root: Path) -> Iterable[SourceUnit]:
        paths = [root] if root.is_file() else sorted(p for p in root.rglob("*") if p.suffix.lower() in FORTRAN_EXTENSIONS)
        for source in paths:
            text = source.read_text(errors="replace")
            lines = text.splitlines()
            fixed_form = source.suffix.lower() in {".f", ".for", ".ftn", ".f77"} or self._looks_fixed_form(lines)
            yield SourceUnit(path=source, lines=lines, fixed_form=fixed_form)

    def _looks_fixed_form(self, lines: List[str]) -> bool:
        signal = 0
        for line in lines[:200]:
            if len(line) > 6 and line[:5].strip().isdigit():
                signal += 1
            if len(line) > 5 and line[5].strip() and not line[:6].lstrip().startswith("!"):
                signal += 1
            if line[:1].upper() in {"C", "*"}:
                signal += 1
        return signal >= 2

    def _index_routines(self, unit: SourceUnit) -> None:
        current: Routine | None = None
        for lineno, raw in enumerate(unit.lines, start=1):
            code = self._strip_comment(raw)
            match = ROUTINE_RE.match(code)
            if match and not re.match(r"^\s*end\b", code, re.IGNORECASE):
                name = match.group(2) or f"{match.group(1).replace(' ', '_')}_{lineno}"
                current = Routine(match.group(2) or name, match.group(1).lower(), str(unit.path), lineno)
                unit.routines.append(current)
            if current is None and code.strip() and not DECLARATION_RE.match(code):
                current = Routine("<main>", "implicit-main", str(unit.path), 1)
                unit.routines.append(current)

            if current:
                if re.search(r"\bimplicit\s+none\b", code, re.IGNORECASE):
                    current.has_implicit_none = True
                if DECLARATION_RE.match(code):
                    current.declarations.update(self._declared_names(code))
                common = COMMON_RE.search(code)
                if common:
                    current.common_blocks.add((common.group(1) or "_blank_").lower())
                label = raw[:5].strip() if unit.fixed_form and len(raw) >= 5 else ""
                if label.isdigit():
                    current.labels.add(label)
                if END_ROUTINE_RE.match(code):
                    current.end_line = lineno
                    current = None

    def _detect_in_unit(self, unit: SourceUnit) -> List[Finding]:
        findings: List[Finding] = []
        if unit.fixed_form:
            findings.append(
                Finding(
                    "fixed-form",
                    "Fixed-form source layout limits refactoring and modern tooling.",
                    Location(str(unit.path), 1),
                    "source-form",
                    effort=Effort.TRIVIAL,
                    safety=Safety.SAFE,
                )
            )

        for lineno, raw in enumerate(unit.lines, start=1):
            code = self._strip_comment(raw)
            routine = self._routine_at(unit, lineno)
            normalized = self._remove_fixed_label(code, unit.fixed_form)

            if re.search(r"\barithmetic\s+if\b", normalized, re.IGNORECASE) or re.search(
                r"^\s*if\s*\([^)]*\)\s*\d+\s*,\s*\d+\s*,\s*\d+\s*$", normalized, re.IGNORECASE
            ):
                findings.append(self._finding(unit, lineno, "arithmetic-if", "Arithmetic IF depends on three-way numeric branch behavior.", raw, routine, Effort.MODERATE, Safety.REVIEW_NEEDED, ["control-flow labels"]))

            if re.search(r"\bgo\s*to\s*\([^)]*\)\s*,?\s*[a-z_]\w*", normalized, re.IGNORECASE):
                findings.append(self._finding(unit, lineno, "computed-goto", "Computed GOTO hides an indexed branch table.", raw, routine, Effort.MODERATE, Safety.REVIEW_NEEDED, ["branch labels", "selector expression"]))

            if COMMON_RE.search(normalized):
                findings.append(self._finding(unit, lineno, "common-block", "COMMON block shares global storage across program units.", raw, routine, Effort.COMPLEX, Safety.RISKY, ["global storage layout", "cross-file declarations"]))

            if EQUIVALENCE_RE.search(normalized):
                findings.append(self._finding(unit, lineno, "equivalence", "EQUIVALENCE overlays storage and may encode aliasing assumptions.", raw, routine, Effort.COMPLEX, Safety.RISKY, ["storage association", "alias analysis"]))

            if ENTRY_RE.match(normalized):
                findings.append(self._finding(unit, lineno, "entry", "ENTRY creates alternate procedure entry points with shared local state.", raw, routine, Effort.COMPLEX, Safety.RISKY, ["call graph", "local variable lifetime"]))

            if ASSUMED_SIZE_RE.search(normalized) and DECLARATION_RE.match(normalized):
                findings.append(self._finding(unit, lineno, "assumed-size-array", "Assumed-size dummy array lacks explicit bounds metadata.", raw, routine, Effort.MODERATE, Safety.REVIEW_NEEDED, ["dummy argument interface"]))

            if self._is_statement_function(normalized, routine):
                findings.append(self._finding(unit, lineno, "statement-function", "Statement function should become an internal procedure or elemental function.", raw, routine, Effort.MODERATE, Safety.SAFE, ["host variables", "function call sites"]))

        for routine in unit.routines:
            if not routine.has_implicit_none:
                findings.append(
                    Finding(
                        "implicit-typing",
                        "Program unit relies on implicit typing rules.",
                        Location(str(unit.path), routine.start_line),
                        "implicit typing",
                        routine=routine.name,
                        effort=Effort.MODERATE if routine.declarations else Effort.TRIVIAL,
                        safety=Safety.REVIEW_NEEDED,
                        dependent_constructs=["undeclared symbols", "implicit type map"],
                    )
                )
        return findings

    def _finding(self, unit: SourceUnit, lineno: int, pattern: str, message: str, raw: str, routine: Routine | None, effort: Effort, safety: Safety, deps: List[str]) -> Finding:
        return Finding(pattern, message, Location(str(unit.path), lineno, max(1, len(raw) - len(raw.lstrip()) + 1)), raw.strip(), routine.name if routine else None, effort, safety, dependent_constructs=deps)

    def _compute_impact(self, files: List[SourceUnit], findings: List[Finding]) -> None:
        common_users: dict[str, set[str]] = defaultdict(set)
        routine_common: dict[tuple[str, str | None], set[str]] = defaultdict(set)
        for unit in files:
            for routine in unit.routines:
                for block in routine.common_blocks:
                    common_users[block].add(str(unit.path))
                    routine_common[(routine.file, routine.name)].add(block)

        total_files = {str(unit.path) for unit in files}
        for finding in findings:
            finding.affected_files.add(finding.location.file)
            if finding.pattern == "common-block":
                block_match = COMMON_RE.search(finding.construct)
                block = (block_match.group(1) if block_match else None) or "_blank_"
                finding.affected_files.update(common_users[block.lower()])
                finding.behavior_risks.append("Changing storage order or initialization can alter behavior.")
                if len(finding.affected_files) > 1:
                    finding.dependent_constructs.append(f"used in {len(finding.affected_files)} files")
            elif finding.pattern == "equivalence":
                finding.behavior_risks.append("Replacement may change aliasing, alignment, or binary I/O layout.")
                finding.affected_files.update(total_files if len(total_files) <= 3 else {finding.location.file})
            elif finding.pattern in {"arithmetic-if", "computed-goto", "entry"}:
                finding.behavior_risks.append("Control-flow restructuring must preserve labels and fall-through behavior.")
            elif finding.pattern == "implicit-typing":
                finding.behavior_risks.append("Adding IMPLICIT NONE requires declarations for every implicit symbol.")
            elif finding.pattern == "assumed-size-array":
                finding.behavior_risks.append("Changing to assumed-shape requires explicit interfaces at call sites.")

    def _prioritize(self, findings: List[Finding]) -> None:
        pattern_weight = {
            "equivalence": 35,
            "common-block": 32,
            "entry": 30,
            "computed-goto": 25,
            "arithmetic-if": 23,
            "implicit-typing": 20,
            "assumed-size-array": 18,
            "statement-function": 12,
            "fixed-form": 8,
        }
        effort_penalty = {Effort.TRIVIAL: 0, Effort.MODERATE: 8, Effort.COMPLEX: 16}
        safety_weight = {Safety.SAFE: 0, Safety.REVIEW_NEEDED: 8, Safety.RISKY: 16}
        for finding in findings:
            finding.priority = (
                pattern_weight.get(finding.pattern, 10)
                + len(finding.affected_files) * 5
                + len(finding.dependent_constructs) * 3
                + safety_weight[finding.safety]
                + effort_penalty[finding.effort]
            )

    def _routine_at(self, unit: SourceUnit, lineno: int) -> Routine | None:
        for routine in unit.routines:
            end = routine.end_line or len(unit.lines)
            if routine.start_line <= lineno <= end:
                return routine
        return None

    def _strip_comment(self, line: str) -> str:
        if line[:1].upper() in {"C", "*"}:
            return ""
        return line.split("!", 1)[0]

    def _remove_fixed_label(self, line: str, fixed_form: bool) -> str:
        if fixed_form and len(line) > 6 and line[:5].strip().isdigit():
            return line[6:]
        return line

    def _declared_names(self, code: str) -> set[str]:
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

    def _is_statement_function(self, code: str, routine: Routine | None) -> bool:
        match = STATEMENT_FUNCTION_RE.match(code)
        if not match or routine is None:
            return False
        name = match.group(1).lower()
        keyword_prefixes = {"if", "do", "call", "where", "forall", "select"}
        if name in routine.declarations:
            return False
        return name not in keyword_prefixes and not DECLARATION_RE.match(code)
