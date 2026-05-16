#include "Analyzer.hpp"

#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <set>
#include <string>

namespace {
void require(bool condition, const std::string &message) {
  if (!condition) {
    std::cerr << "FAIL: " << message << "\n";
    std::exit(1);
  }
}

std::set<std::string> patterns(const ProjectAnalysis &analysis) {
  std::set<std::string> result;
  for (const auto &finding : analysis.findings) {
    result.insert(finding.pattern);
  }
  return result;
}
} // namespace

int main() {
  auto root = std::filesystem::current_path();
  ModernizationAnalyzer analyzer;

  auto legacy = analyzer.analyzePath(root / "examples" / "legacy");
  auto found = patterns(legacy);
  require(found.count("arithmetic-if") == 1, "detect arithmetic IF");
  require(found.count("computed-goto") == 1, "detect computed GOTO");
  require(found.count("equivalence") == 1, "detect EQUIVALENCE");
  require(found.count("common-block") == 1, "detect COMMON");
  require(found.count("implicit-typing") == 1, "detect implicit typing");
  require(found.count("statement-function") == 1, "detect statement function");
  require(found.count("fixed-form") == 1, "detect fixed form");
  require(found.count("assumed-size-array") == 1, "detect assumed-size array");
  require(found.count("entry") == 1, "detect ENTRY");


  auto falsePositives = analyzer.analyzePath(root / "examples" / "legacy" / "false_positives.f90");
  auto falsePatterns = patterns(falsePositives);
  require(falsePatterns.count("arithmetic-if") == 0, "ordinary IF THEN is not arithmetic IF");
  require(falsePatterns.count("computed-goto") == 0, "ordinary GOTO is not computed GOTO");
  require(falsePatterns.count("statement-function") == 0, "array assignment is not statement function");

  auto caseStudy = analyzer.analyzePath(root / "examples" / "case_study");
  bool multiFileCommon = false;
  bool riskyTop = false;
  int topPriority = -1;
  std::string topPattern;
  for (const auto &finding : caseStudy.findings) {
    if (finding.pattern == "common-block" && finding.affectedFiles.size() == 2) {
      multiFileCommon = true;
    }
    if (finding.priority > topPriority) {
      topPriority = finding.priority;
      topPattern = finding.pattern;
    }
  }
  riskyTop = topPattern == "common-block" || topPattern == "equivalence";
  require(multiFileCommon, "COMMON impact spans multiple files");
  require(riskyTop, "risky storage constructs rank highest");

  std::cout << "All C++ analyzer tests passed.\n";
  return 0;
}
