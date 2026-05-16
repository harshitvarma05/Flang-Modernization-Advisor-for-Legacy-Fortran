#include "Findings.hpp"

#include <algorithm>
#include <sstream>

namespace advisor {

std::string toString(Effort effort) {
  switch (effort) {
  case Effort::Trivial:
    return "trivial";
  case Effort::Moderate:
    return "moderate";
  case Effort::Complex:
    return "complex";
  }
  return "moderate";
}

std::string toString(Safety safety) {
  switch (safety) {
  case Safety::Safe:
    return "safe";
  case Safety::ReviewNeeded:
    return "review-needed";
  case Safety::Risky:
    return "risky";
  }
  return "review-needed";
}

static int patternWeight(const std::string &pattern) {
  if (pattern == "equivalence")
    return 34;
  if (pattern == "common")
    return 31;
  if (pattern == "entry")
    return 29;
  if (pattern == "computed-goto")
    return 24;
  if (pattern == "arithmetic-if")
    return 23;
  if (pattern == "assumed-size-array")
    return 20;
  if (pattern == "implicit-typing")
    return 17;
  if (pattern == "statement-function")
    return 14;
  if (pattern == "fixed-form")
    return 8;
  return 10;
}

static int effortPenalty(Effort effort) {
  switch (effort) {
  case Effort::Trivial:
    return -2;
  case Effort::Moderate:
    return 6;
  case Effort::Complex:
    return 13;
  }
  return 0;
}

static int safetyWeight(Safety safety) {
  switch (safety) {
  case Safety::Safe:
    return 0;
  case Safety::ReviewNeeded:
    return 11;
  case Safety::Risky:
    return 22;
  }
  return 0;
}

void prioritize(ProjectAnalysis &analysis) {
  for (auto &finding : analysis.findings) {
    finding.priority = patternWeight(finding.pattern) +
        static_cast<int>(finding.affectedFiles.size()) * 4 +
        static_cast<int>(finding.dependentConstructs.size()) * 3 +
        static_cast<int>(finding.behaviorRisks.size()) * 5 +
        effortPenalty(finding.effort) + safetyWeight(finding.safety);
  }
  std::sort(analysis.findings.begin(), analysis.findings.end(),
      [](const Finding &a, const Finding &b) {
        if (a.priority != b.priority)
          return a.priority > b.priority;
        return a.pattern < b.pattern;
      });
}

static std::string escapeJson(const std::string &value) {
  std::ostringstream out;
  for (char ch : value) {
    switch (ch) {
    case '"':
      out << "\\\"";
      break;
    case '\\':
      out << "\\\\";
      break;
    case '\n':
      out << "\\n";
      break;
    default:
      out << ch;
    }
  }
  return out.str();
}

static void writeStringArray(
    std::ostringstream &out, const std::vector<std::string> &values) {
  out << "[";
  for (std::size_t i = 0; i < values.size(); ++i) {
    if (i)
      out << ", ";
    out << "\"" << escapeJson(values[i]) << "\"";
  }
  out << "]";
}

static void writeStringSet(
    std::ostringstream &out, const std::set<std::string> &values) {
  out << "[";
  std::size_t i = 0;
  for (const auto &value : values) {
    if (i++)
      out << ", ";
    out << "\"" << escapeJson(value) << "\"";
  }
  out << "]";
}

std::string jsonReport(const ProjectAnalysis &analysis) {
  std::ostringstream out;
  out << "{\n";
  out << "  \"root\": \"" << escapeJson(analysis.root) << "\",\n";
  out << "  \"files_analyzed\": " << analysis.files.size() << ",\n";
  out << "  \"findings\": [\n";
  for (std::size_t i = 0; i < analysis.findings.size(); ++i) {
    const auto &f = analysis.findings[i];
    out << "    {\n";
    out << "      \"pattern\": \"" << escapeJson(f.pattern) << "\",\n";
    out << "      \"message\": \"" << escapeJson(f.message) << "\",\n";
    out << "      \"file\": \"" << escapeJson(f.location.file) << "\",\n";
    out << "      \"line\": " << f.location.line << ",\n";
    out << "      \"routine\": \"" << escapeJson(f.routine) << "\",\n";
    out << "      \"effort\": \"" << toString(f.effort) << "\",\n";
    out << "      \"safety\": \"" << toString(f.safety) << "\",\n";
    out << "      \"priority\": " << f.priority << ",\n";
    out << "      \"affected_files\": ";
    writeStringSet(out, f.affectedFiles);
    out << ",\n";
    out << "      \"dependent_constructs\": ";
    writeStringArray(out, f.dependentConstructs);
    out << ",\n";
    out << "      \"behavior_risks\": ";
    writeStringArray(out, f.behaviorRisks);
    out << ",\n";
    out << "      \"flang_evidence\": ";
    writeStringArray(out, f.flangEvidence);
    out << ",\n";
    out << "      \"semantic_evidence\": ";
    writeStringArray(out, f.semanticEvidence);
    out << "\n    }" << (i + 1 == analysis.findings.size() ? "\n" : ",\n");
  }
  out << "  ]\n";
  out << "}\n";
  return out.str();
}

std::string markdownReport(const ProjectAnalysis &analysis) {
  std::map<std::string, int> summary;
  for (const auto &finding : analysis.findings)
    ++summary[finding.pattern];

  std::ostringstream out;
  out << "# In-Memory Flang AST Modernization Report\n\n";
  out << "Root: `" << analysis.root << "`\n\n";
  out << "Files analyzed: " << analysis.files.size() << "\n\n";
  out << "Total findings: " << analysis.findings.size() << "\n\n";
  out << "## Pattern Summary\n\n";
  out << "| Pattern | Count |\n|---|---:|\n";
  for (const auto &[pattern, count] : summary)
    out << "| " << pattern << " | " << count << " |\n";

  out << "\n## Prioritized Modernization Plan\n\n";
  out << "| Priority | Pattern | Location | Effort | Safety | Recommendation |\n";
  out << "|---:|---|---|---|---|---|\n";
  for (const auto &f : analysis.findings) {
    out << "| " << f.priority << " | " << f.pattern << " | "
        << f.location.file << ":" << f.location.line << " | "
        << toString(f.effort) << " | " << toString(f.safety) << " | "
        << f.message << " |\n";
  }

  out << "\n## Flang Evidence And Impact Details\n\n";
  for (const auto &f : analysis.findings) {
    out << "### " << f.pattern << " at `" << f.location.file << ":"
        << f.location.line << "`\n\n";
    out << "- Effort: " << toString(f.effort) << "\n";
    out << "- Safety: " << toString(f.safety) << "\n";
    out << "- Construct: `" << f.construct << "`\n";
    if (!f.routine.empty())
      out << "- Routine/scope: `" << f.routine << "`\n";
    if (!f.affectedFiles.empty()) {
      out << "- Affected files:";
      for (const auto &file : f.affectedFiles)
        out << " `" << file << "`";
      out << "\n";
    }
    for (const auto &dep : f.dependentConstructs)
      out << "- Dependent construct: " << dep << "\n";
    for (const auto &risk : f.behaviorRisks)
      out << "- Behavior/aliasing risk: " << risk << "\n";
    for (const auto &evidence : f.flangEvidence)
      out << "- Flang AST evidence: " << evidence << "\n";
    for (const auto &evidence : f.semanticEvidence)
      out << "- Flang semantic evidence: " << evidence << "\n";
    out << "\n";
  }
  return out.str();
}

} // namespace advisor
