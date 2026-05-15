import unittest
from pathlib import Path

from flang_modernizer import ModernizationAnalyzer


ROOT = Path(__file__).resolve().parents[1]


class AnalyzerTests(unittest.TestCase):
    def test_detects_required_patterns(self):
        analysis = ModernizationAnalyzer().analyze_path(ROOT / "examples" / "legacy")
        patterns = {finding.pattern for finding in analysis.findings}
        self.assertIn("arithmetic-if", patterns)
        self.assertIn("computed-goto", patterns)
        self.assertIn("equivalence", patterns)
        self.assertIn("common-block", patterns)
        self.assertIn("implicit-typing", patterns)
        self.assertIn("statement-function", patterns)
        self.assertIn("fixed-form", patterns)
        self.assertIn("assumed-size-array", patterns)
        self.assertIn("entry", patterns)

    def test_common_block_impact_spans_multiple_files(self):
        analysis = ModernizationAnalyzer().analyze_path(ROOT / "examples" / "case_study")
        common_findings = [f for f in analysis.findings if f.pattern == "common-block"]
        self.assertTrue(common_findings)
        self.assertTrue(any(len(f.affected_files) == 2 for f in common_findings))

    def test_risky_constructs_rank_high(self):
        analysis = ModernizationAnalyzer().analyze_path(ROOT / "examples" / "case_study")
        top = analysis.sorted_findings()[0]
        self.assertIn(top.pattern, {"common-block", "equivalence"})


if __name__ == "__main__":
    unittest.main()
