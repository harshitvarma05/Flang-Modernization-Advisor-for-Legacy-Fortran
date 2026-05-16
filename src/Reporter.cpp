#include "Reporter.hpp"

#include <algorithm>
#include <map>
#include <sstream>

namespace {
std::string location(const Location &loc) {
  return loc.file + ":" + std::to_string(loc.line) + ":" + std::to_string(loc.column);
}

std::string join(const std::vector<std::string> &items) {
  if (items.empty()) {
    return "";
  }
  std::ostringstream out;
  for (size_t i = 0; i < items.size(); ++i) {
    if (i > 0) {
      out << ", ";
    }
    out << items[i];
  }
  return out.str();
}

std::string recommendation(const std::string &pattern) {
  if (pattern == "arithmetic-if") {
    return "Replace with explicit IF/ELSE IF/ELSE branches after checking label fall-through.";
  }
  if (pattern == "computed-goto") {
    return "Replace branch table with SELECT CASE or procedure dispatch.";
  }
  if (pattern == "equivalence") {
    return "Isolate storage overlay and replace only after aliasing/binary-layout review.";
  }
  if (pattern == "common-block") {
    return "Migrate shared state to a module, preserving declaration order and initialization.";
  }
  if (pattern == "implicit-typing") {
    return "Add IMPLICIT NONE and explicit declarations per program unit.";
  }
  if (pattern == "statement-function") {
    return "Convert to an internal or module procedure.";
  }
  if (pattern == "fixed-form") {
    return "Convert to free-form source and normalize continuations/comments.";
  }
  if (pattern == "assumed-size-array") {
    return "Use assumed-shape arrays after introducing explicit interfaces.";
  }
  if (pattern == "entry") {
    return "Split alternate entries into separate procedures with explicit shared state.";
  }
  return "Review construct and modernize locally.";
}

std::vector<Finding> sortedFindings(ProjectAnalysis analysis) {
  auto findings = analysis.findings;
  std::sort(findings.begin(), findings.end(), [](const Finding &left, const Finding &right) {
    if (left.priority != right.priority) {
      return left.priority > right.priority;
    }
    if (left.location.file != right.location.file) {
      return left.location.file < right.location.file;
    }
    return left.location.line < right.location.line;
  });
  return findings;
}

std::string escapeJson(const std::string &value) {
  std::string out;
  for (char ch : value) {
    if (ch == '"' || ch == '\\') {
      out.push_back('\\');
    }
    if (ch == '\n') {
      out += "\\n";
    } else {
      out.push_back(ch);
    }
  }
  return out;
}
} // namespace

std::string reportMarkdown(ProjectAnalysis analysis) {
  std::map<std::string, int> counts;
  for (const auto &finding : analysis.findings) {
    counts[finding.pattern]++;
  }

  std::ostringstream out;
  out << "# Flang Modernization Advisor Report\n\n";
  out << "Analyzed root: `" << analysis.root << "`\n";
  out << "Files analyzed: **" << analysis.files.size() << "**\n";
  out << "Findings: **" << analysis.findings.size() << "**\n\n";
  out << "## Pattern Summary\n\n";
  out << "| Pattern | Count |\n|---|---:|\n";
  for (const auto &[pattern, count] : counts) {
    out << "| " << pattern << " | " << count << " |\n";
  }

  out << "\n## Prioritized Modernization Plan\n\n";
  out << "| Priority | Pattern | Location | Effort | Safety | Recommendation |\n";
  out << "|---:|---|---|---|---|---|\n";
  for (const auto &finding : sortedFindings(analysis)) {
    out << "| " << finding.priority << " | " << finding.pattern << " | `" << location(finding.location) << "` | "
        << toString(finding.effort) << " | " << toString(finding.safety) << " | "
        << recommendation(finding.pattern) << " |\n";
  }

  out << "\n## Impact Details\n\n";
  for (const auto &finding : sortedFindings(analysis)) {
    std::vector<std::string> files(finding.affectedFiles.begin(), finding.affectedFiles.end());
    out << "### " << finding.pattern << " at `" << location(finding.location) << "`\n";
    out << "- Routine: `" << (finding.routine.empty() ? "file scope" : finding.routine) << "`\n";
    out << "- Affected files: " << files.size() << "\n";
    out << "- Dependent constructs: " << (finding.dependentConstructs.empty() ? "none recorded" : join(finding.dependentConstructs)) << "\n";
    out << "- Behavior risks: " << (finding.behaviorRisks.empty() ? "low" : join(finding.behaviorRisks)) << "\n";
    out << "- Flang parse-tree evidence: " << (finding.flangEvidence.empty() ? "not available" : join(finding.flangEvidence)) << "\n";
    out << "- Flang semantic evidence: " << (finding.semanticEvidence.empty() ? "not available" : join(finding.semanticEvidence)) << "\n\n";
  }
  return out.str();
}

std::string reportJson(ProjectAnalysis analysis) {
  std::ostringstream out;
  auto findings = sortedFindings(analysis);
  out << "{\n  \"root\": \"" << escapeJson(analysis.root) << "\",\n";
  out << "  \"files_analyzed\": " << analysis.files.size() << ",\n";
  out << "  \"findings\": [\n";
  for (size_t i = 0; i < findings.size(); ++i) {
    const auto &finding = findings[i];
    out << "    {\n";
    out << "      \"pattern\": \"" << escapeJson(finding.pattern) << "\",\n";
    out << "      \"message\": \"" << escapeJson(finding.message) << "\",\n";
    out << "      \"location\": \"" << escapeJson(location(finding.location)) << "\",\n";
    out << "      \"routine\": \"" << escapeJson(finding.routine) << "\",\n";
    out << "      \"effort\": \"" << toString(finding.effort) << "\",\n";
    out << "      \"safety\": \"" << toString(finding.safety) << "\",\n";
    out << "      \"priority\": " << finding.priority << ",\n";
    out << "      \"flang_evidence_count\": " << finding.flangEvidence.size() << ",\n";
    out << "      \"semantic_evidence_count\": " << finding.semanticEvidence.size() << "\n";
    out << "    }" << (i + 1 == findings.size() ? "\n" : ",\n");
  }
  out << "  ]\n}\n";
  return out.str();
}
