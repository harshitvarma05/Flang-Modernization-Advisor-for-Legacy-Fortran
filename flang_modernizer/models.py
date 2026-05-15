from __future__ import annotations

from dataclasses import dataclass, field
from enum import Enum
from pathlib import Path
from typing import Dict, List, Set


class Effort(str, Enum):
    TRIVIAL = "trivial"
    MODERATE = "moderate"
    COMPLEX = "complex"


class Safety(str, Enum):
    SAFE = "safe"
    REVIEW_NEEDED = "review-needed"
    RISKY = "risky"


@dataclass(frozen=True)
class Location:
    file: str
    line: int
    column: int = 1

    def display(self) -> str:
        return f"{self.file}:{self.line}:{self.column}"


@dataclass
class Finding:
    pattern: str
    message: str
    location: Location
    construct: str
    routine: str | None = None
    effort: Effort = Effort.MODERATE
    safety: Safety = Safety.REVIEW_NEEDED
    affected_files: Set[str] = field(default_factory=set)
    dependent_constructs: List[str] = field(default_factory=list)
    behavior_risks: List[str] = field(default_factory=list)
    priority: int = 50

    def as_dict(self) -> Dict[str, object]:
        return {
            "pattern": self.pattern,
            "message": self.message,
            "location": self.location.display(),
            "routine": self.routine,
            "effort": self.effort.value,
            "safety": self.safety.value,
            "affected_files": sorted(self.affected_files),
            "dependent_constructs": self.dependent_constructs,
            "behavior_risks": self.behavior_risks,
            "priority": self.priority,
        }


@dataclass
class Routine:
    name: str
    kind: str
    file: str
    start_line: int
    end_line: int | None = None
    has_implicit_none: bool = False
    declarations: Set[str] = field(default_factory=set)
    common_blocks: Set[str] = field(default_factory=set)
    labels: Set[str] = field(default_factory=set)


@dataclass
class SourceUnit:
    path: Path
    lines: List[str]
    fixed_form: bool
    routines: List[Routine] = field(default_factory=list)


@dataclass
class ProjectAnalysis:
    root: Path
    files: List[SourceUnit]
    findings: List[Finding]

    def sorted_findings(self) -> List[Finding]:
        return sorted(self.findings, key=lambda f: (-f.priority, f.location.file, f.location.line))
