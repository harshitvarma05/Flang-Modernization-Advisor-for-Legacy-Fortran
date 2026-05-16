#include "Analyzer.hpp"
#include "FlangMetadata.hpp"

#include <algorithm>
#include <fstream>
#include <map>
#include <regex>
#include <sstream>

namespace {
const std::regex DeclarationRe(
    R"(^\s*(integer|real|double\s+precision|complex|logical|character|type\s*\(|class\s*\())",
    std::regex::icase);
const std::regex RoutineRe(R"(^\s*(program|subroutine|function|block\s+data)\s+([a-z_]\w*)?)",
                           std::regex::icase);
const std::regex EndRoutineRe(R"(^\s*end\s*(program|subroutine|function|block\s+data)?\b)",
                              std::regex::icase);
const std::regex CommonRe(R"(\bcommon\s*(?:/\s*([a-z_]\w*)\s*/)?\s*(.*))", std::regex::icase);
const std::regex EquivalenceRe(R"(\bequivalence\s*\((.*)\))", std::regex::icase);
const std::regex AssumedSizeRe(R"(\b[a-z_]\w*\s*\([^)]*\*[^)]*\))", std::regex::icase);
const std::regex StatementFunctionRe(R"(^\s*([a-z_]\w*)\s*\(([^)]*)\)\s*=)", std::regex::icase);
const std::regex EntryRe(R"(^\s*entry\s+([a-z_]\w*))", std::regex::icase);
const std::regex ArithmeticIfRe(R"(^\s*if\s*\([^)]*\)\s*\d+\s*,\s*\d+\s*,\s*\d+\s*$)",
                                std::regex::icase);
const std::regex ComputedGotoRe(R"(\bgo\s*to\s*\([^)]*\)\s*,?\s*[a-z_]\w*)", std::regex::icase);

std::vector<std::string> readLines(const std::filesystem::path &path) {
  std::ifstream input(path);
  std::vector<std::string> lines;
  std::string line;
  while (std::getline(input, line)) {
    lines.push_back(line);
  }
  return lines;
}

std::string trimLocal(const std::string &value) {
  auto start = value.find_first_not_of(" \t\r\n");
  if (start == std::string::npos) {
    return "";
  }
  auto end = value.find_last_not_of(" \t\r\n");
  return value.substr(start, end - start + 1);
}

std::string joinInts(const std::vector<std::string> &items) {
  std::ostringstream out;
  for (size_t i = 0; i < items.size(); ++i) {
    if (i) out << ", ";
    out << items[i];
  }
  return out.str();
}

bool isSpecificationLike(const std::string &code) {
  std::string t = trimLocal(code);
  if (t.empty()) return true;
  if (std::regex_search(t, DeclarationRe) || std::regex_search(t, CommonRe) || std::regex_search(t, EquivalenceRe) ||
      std::regex_search(t, EntryRe) || std::regex_search(t, std::regex(R"(^\s*implicit)", std::regex::icase)) ||
      std::regex_search(t, StatementFunctionRe)) {
    return true;
  }
  return false;
}

Finding makeFinding(const SourceUnit &unit, int line, const std::string &pattern, const std::string &message,
                    const std::string &raw, const Routine *routine, Effort effort, Safety safety,
                    std::vector<std::string> deps) {
  auto first = raw.find_first_not_of(" \t");
  int column = first == std::string::npos ? 1 : static_cast<int>(first) + 1;
  Finding finding;
  finding.pattern = pattern;
  finding.message = message;
  finding.location = {unit.path, line, column};
  finding.construct = trimLocal(raw);
  finding.routine = routine ? routine->name : "";
  finding.effort = effort;
  finding.safety = safety;
  finding.dependentConstructs = std::move(deps);
  return finding;
}
} // namespace

ProjectAnalysis ModernizationAnalyzer::analyzePath(const std::filesystem::path &path) {
  ProjectAnalysis analysis;
  analysis.root = std::filesystem::absolute(path).string();
  analysis.files = loadSources(path);
  for (auto &unit : analysis.files) {
    indexRoutines(unit);
  }
  for (const auto &unit : analysis.files) {
    auto unitFindings = detectInUnit(unit);
    analysis.findings.insert(analysis.findings.end(), unitFindings.begin(), unitFindings.end());
  }
  computeImpact(analysis.files, analysis.findings);
  enrichWithFlangMetadata(FlangMetadataProvider().collect(analysis.files), analysis.findings);
  prioritize(analysis.findings);
  return analysis;
}

std::vector<SourceUnit> ModernizationAnalyzer::loadSources(const std::filesystem::path &root) {
  std::vector<std::filesystem::path> paths;
  if (std::filesystem::is_regular_file(root)) {
    paths.push_back(root);
  } else {
    for (const auto &entry : std::filesystem::recursive_directory_iterator(root)) {
      if (entry.is_regular_file() && isFortranFile(entry.path())) {
        paths.push_back(entry.path());
      }
    }
  }
  std::sort(paths.begin(), paths.end());

  std::vector<SourceUnit> units;
  for (const auto &path : paths) {
    SourceUnit unit;
    unit.path = std::filesystem::absolute(path).string();
    unit.lines = readLines(path);
    auto ext = lower(path.extension().string());
    unit.fixedForm = ext == ".f" || ext == ".for" || ext == ".ftn" || ext == ".f77" ||
                     looksFixedForm(unit.lines);
    units.push_back(std::move(unit));
  }
  return units;
}

void ModernizationAnalyzer::indexRoutines(SourceUnit &unit) {
  Routine *current = nullptr;
  for (int index = 0; index < static_cast<int>(unit.lines.size()); ++index) {
    int lineNumber = index + 1;
    std::string code = stripComment(unit.lines[index]);
    std::smatch match;
    if (std::regex_search(code, match, RoutineRe) && !std::regex_search(code, std::regex(R"(^\s*end\b)", std::regex::icase))) {
      Routine routine;
      routine.kind = lower(match[1].str());
      routine.name = match[2].matched ? match[2].str() : routine.kind + "_" + std::to_string(lineNumber);
      routine.file = unit.path;
      routine.startLine = lineNumber;
      unit.routines.push_back(routine);
      current = &unit.routines.back();
    }
    if (!current && !trim(code).empty() && !std::regex_search(code, DeclarationRe)) {
      Routine routine;
      routine.kind = "implicit-main";
      routine.name = "<main>";
      routine.file = unit.path;
      routine.startLine = 1;
      unit.routines.push_back(routine);
      current = &unit.routines.back();
    }
    if (current) {
      if (std::regex_search(code, std::regex(R"(\bimplicit\s+none\b)", std::regex::icase))) {
        current->hasImplicitNone = true;
      }
      if (std::regex_search(code, DeclarationRe)) {
        auto names = declaredNames(code);
        current->declarations.insert(names.begin(), names.end());
      }
      if (std::regex_search(code, match, CommonRe)) {
        current->commonBlocks.insert(match[1].matched ? lower(match[1].str()) : "_blank_");
      }
      if (current->firstExecutableLine == 0 && lineNumber != current->startLine && !isSpecificationLike(code) && !std::regex_search(code, EndRoutineRe)) {
        current->firstExecutableLine = lineNumber;
      }
      if (std::regex_search(code, EndRoutineRe)) {
        current->endLine = lineNumber;
        current = nullptr;
      }
    }
  }
}

std::vector<Finding> ModernizationAnalyzer::detectInUnit(const SourceUnit &unit) {
  std::vector<Finding> findings;
  if (unit.fixedForm) {
    Finding finding;
    finding.pattern = "fixed-form";
    finding.message = "Fixed-form source layout limits refactoring and modern tooling.";
    finding.location = {unit.path, 1, 1};
    finding.construct = "source-form";
    finding.effort = Effort::Trivial;
    finding.safety = Safety::Safe;
    findings.push_back(finding);
  }

  for (int index = 0; index < static_cast<int>(unit.lines.size()); ++index) {
    int lineNumber = index + 1;
    const std::string &raw = unit.lines[index];
    std::string normalized = removeFixedLabel(stripComment(raw), unit.fixedForm);
    const Routine *routine = routineAt(unit, lineNumber);

    std::smatch branchMatch;
    if (std::regex_search(normalized, branchMatch, ArithmeticIfRe)) {
      std::vector<std::string> labels;
      std::regex labelRe(R"(\d+)");
      for (auto it = std::sregex_iterator(normalized.begin(), normalized.end(), labelRe); it != std::sregex_iterator(); ++it) {
        labels.push_back(it->str());
      }
      findings.push_back(makeFinding(unit, lineNumber, "arithmetic-if",
                                     "Arithmetic IF depends on three-way numeric branch behavior.", raw, routine,
                                     Effort::Moderate, Safety::ReviewNeeded,
                                     {"target labels: " + joinInts(labels)}));
    }
    if (std::regex_search(normalized, ComputedGotoRe)) {
      std::vector<std::string> labels;
      auto open = normalized.find('(');
      auto close = normalized.find(')', open == std::string::npos ? 0 : open);
      if (open != std::string::npos && close != std::string::npos) {
        std::string labelList = normalized.substr(open + 1, close - open - 1);
        std::regex labelRe(R"(\d+)");
        for (auto it = std::sregex_iterator(labelList.begin(), labelList.end(), labelRe); it != std::sregex_iterator(); ++it) {
          labels.push_back(it->str());
        }
      }
      findings.push_back(makeFinding(unit, lineNumber, "computed-goto", "Computed GOTO hides an indexed branch table.",
                                     raw, routine, Effort::Moderate, Safety::ReviewNeeded,
                                     {"target labels: " + joinInts(labels), "selector expression"}));
    }
    if (std::regex_search(normalized, CommonRe)) {
      findings.push_back(makeFinding(unit, lineNumber, "common-block",
                                     "COMMON block shares global storage across program units.", raw, routine,
                                     Effort::Complex, Safety::Risky,
                                     {"global storage layout", "cross-file declarations"}));
    }
    if (std::regex_search(normalized, EquivalenceRe)) {
      findings.push_back(makeFinding(unit, lineNumber, "equivalence",
                                     "EQUIVALENCE overlays storage and may encode aliasing assumptions.", raw, routine,
                                     Effort::Complex, Safety::Risky, {"storage association", "alias analysis"}));
    }
    if (std::regex_search(normalized, EntryRe)) {
      findings.push_back(makeFinding(unit, lineNumber, "entry",
                                     "ENTRY creates alternate procedure entry points with shared local state.", raw,
                                     routine, Effort::Complex, Safety::Risky,
                                     {"call graph", "local variable lifetime"}));
    }
    if (std::regex_search(normalized, AssumedSizeRe) && std::regex_search(normalized, DeclarationRe)) {
      findings.push_back(makeFinding(unit, lineNumber, "assumed-size-array",
                                     "Assumed-size dummy array lacks explicit bounds metadata.", raw, routine,
                                     Effort::Moderate, Safety::ReviewNeeded, {"dummy argument interface"}));
    }
    if (isStatementFunction(normalized, routine) && (!routine || routine->firstExecutableLine == 0 || lineNumber < routine->firstExecutableLine)) {
      findings.push_back(makeFinding(unit, lineNumber, "statement-function",
                                     "Statement function should become an internal procedure or elemental function.",
                                     raw, routine, Effort::Moderate, Safety::Safe,
                                     {"host variables", "function call sites"}));
    }
  }

  for (const auto &routine : unit.routines) {
    if (!routine.hasImplicitNone) {
      Finding finding;
      finding.pattern = "implicit-typing";
      finding.message = "Program unit relies on implicit typing rules.";
      finding.location = {unit.path, routine.startLine, 1};
      finding.construct = "implicit typing";
      finding.routine = routine.name;
      finding.effort = routine.declarations.empty() ? Effort::Trivial : Effort::Moderate;
      finding.safety = Safety::ReviewNeeded;
      finding.dependentConstructs = {"undeclared symbols", "implicit type map"};
      findings.push_back(finding);
    }
  }
  return findings;
}

void ModernizationAnalyzer::computeImpact(const std::vector<SourceUnit> &files, std::vector<Finding> &findings) {
  std::map<std::string, std::set<std::string>> commonUsers;
  std::set<std::string> totalFiles;
  for (const auto &unit : files) {
    totalFiles.insert(unit.path);
    for (const auto &routine : unit.routines) {
      for (const auto &block : routine.commonBlocks) {
        commonUsers[block].insert(unit.path);
      }
    }
  }

  for (auto &finding : findings) {
    finding.affectedFiles.insert(finding.location.file);
    if (finding.pattern == "common-block") {
      std::smatch match;
      std::string block = "_blank_";
      if (std::regex_search(finding.construct, match, CommonRe) && match[1].matched) {
        block = lower(match[1].str());
      }
      finding.affectedFiles.insert(commonUsers[block].begin(), commonUsers[block].end());
      finding.behaviorRisks.push_back("Changing storage order or initialization can alter behavior.");
      if (finding.affectedFiles.size() > 1) {
        finding.dependentConstructs.push_back("used in " + std::to_string(finding.affectedFiles.size()) + " files");
      }
    } else if (finding.pattern == "equivalence") {
      finding.behaviorRisks.push_back("Replacement may change aliasing, alignment, or binary I/O layout.");
      if (totalFiles.size() <= 3) {
        finding.affectedFiles.insert(totalFiles.begin(), totalFiles.end());
      }
    } else if (finding.pattern == "arithmetic-if" || finding.pattern == "computed-goto" ||
               finding.pattern == "entry") {
      finding.behaviorRisks.push_back("Control-flow restructuring must preserve labels and fall-through behavior.");
    } else if (finding.pattern == "implicit-typing") {
      finding.behaviorRisks.push_back("Adding IMPLICIT NONE requires declarations for every implicit symbol.");
    } else if (finding.pattern == "assumed-size-array") {
      finding.behaviorRisks.push_back("Changing to assumed-shape requires explicit interfaces at call sites.");
    }
  }
}

void ModernizationAnalyzer::enrichWithFlangMetadata(const std::map<std::string, FlangFileMetadata> &metadata, std::vector<Finding> &findings) {
  for (auto &finding : findings) {
    auto it = metadata.find(finding.location.file);
    if (it == metadata.end()) {
      continue;
    }
    const auto &fileMeta = it->second;
    if (fileMeta.parseTreeAvailable && fileMeta.parseConstructs.count(finding.pattern)) {
      finding.flangEvidence.push_back("Flang parse tree confirms " + finding.pattern + " in this source file.");
    }
    if (fileMeta.symbolsAvailable) {
      for (const auto &evidence : fileMeta.symbolEvidence) {
        if ((finding.pattern == "common-block" && evidence.find("COMMON") != std::string::npos) ||
            (finding.pattern == "equivalence" && evidence.find("equivalence") != std::string::npos) ||
            (finding.pattern == "assumed-size-array" && evidence.find("assumed-size") != std::string::npos) ||
            (finding.pattern == "statement-function" && evidence.find("statement function") != std::string::npos)) {
          finding.semanticEvidence.push_back(evidence);
        }
      }
    }
  }
}

void ModernizationAnalyzer::prioritize(std::vector<Finding> &findings) {
  std::map<std::string, int> patternWeight{{"equivalence", 35},       {"common-block", 32},
                                           {"entry", 30},             {"computed-goto", 25},
                                           {"arithmetic-if", 23},     {"implicit-typing", 20},
                                           {"assumed-size-array", 18}, {"statement-function", 12},
                                           {"fixed-form", 8}};
  for (auto &finding : findings) {
    int effortPenalty = finding.effort == Effort::Trivial ? 0 : finding.effort == Effort::Moderate ? 8 : 16;
    int safetyWeight = finding.safety == Safety::Safe ? 0 : finding.safety == Safety::ReviewNeeded ? 8 : 16;
    finding.priority = patternWeight[finding.pattern] + static_cast<int>(finding.affectedFiles.size()) * 5 +
                       static_cast<int>(finding.dependentConstructs.size()) * 3 + effortPenalty + safetyWeight;
  }
}

bool ModernizationAnalyzer::isFortranFile(const std::filesystem::path &path) {
  std::string ext = lower(path.extension().string());
  return ext == ".f" || ext == ".for" || ext == ".ftn" || ext == ".f77" || ext == ".f90" || ext == ".f95" ||
         ext == ".f03" || ext == ".f08";
}

bool ModernizationAnalyzer::looksFixedForm(const std::vector<std::string> &lines) {
  int signal = 0;
  for (size_t i = 0; i < std::min<size_t>(lines.size(), 200); ++i) {
    const auto &line = lines[i];
    if (line.size() > 6 && trim(line.substr(0, 5)).find_first_not_of("0123456789") == std::string::npos &&
        !trim(line.substr(0, 5)).empty()) {
      ++signal;
    }
    if (line.size() > 5 && !trim(line.substr(5, 1)).empty()) {
      ++signal;
    }
    if (!line.empty() && (line[0] == 'C' || line[0] == 'c' || line[0] == '*')) {
      ++signal;
    }
  }
  return signal >= 2;
}

std::string ModernizationAnalyzer::stripComment(const std::string &line) {
  if (!line.empty() && (line[0] == 'C' || line[0] == 'c' || line[0] == '*')) {
    return "";
  }
  auto bang = line.find('!');
  return bang == std::string::npos ? line : line.substr(0, bang);
}

std::string ModernizationAnalyzer::removeFixedLabel(const std::string &line, bool fixedForm) {
  if (fixedForm && line.size() > 6) {
    std::string label = trim(line.substr(0, 5));
    if (!label.empty() && label.find_first_not_of("0123456789") == std::string::npos) {
      return line.substr(6);
    }
  }
  return line;
}

std::string ModernizationAnalyzer::trim(const std::string &value) {
  auto start = value.find_first_not_of(" \t\r\n");
  if (start == std::string::npos) {
    return "";
  }
  auto end = value.find_last_not_of(" \t\r\n");
  return value.substr(start, end - start + 1);
}

std::string ModernizationAnalyzer::lower(std::string value) {
  std::transform(value.begin(), value.end(), value.begin(), [](unsigned char ch) { return std::tolower(ch); });
  return value;
}

std::set<std::string> ModernizationAnalyzer::declaredNames(const std::string &code) {
  std::string tail = code;
  auto pos = tail.find("::");
  if (pos != std::string::npos) {
    tail = tail.substr(pos + 2);
  } else {
    tail = std::regex_replace(tail,
                              std::regex(R"(^\s*(integer|real|complex|logical|character|double\s+precision)(?:\s*\*?\s*\d+|\s*\([^)]*\))?\s*)",
                                         std::regex::icase),
                              "");
  }
  std::set<std::string> names;
  std::stringstream stream(tail);
  std::string part;
  while (std::getline(stream, part, ',')) {
    auto paren = part.find('(');
    if (paren != std::string::npos) {
      part = part.substr(0, paren);
    }
    part = lower(trim(part));
    if (std::regex_match(part, std::regex(R"([a-z_]\w*)", std::regex::icase))) {
      names.insert(part);
    }
  }
  return names;
}

const Routine *ModernizationAnalyzer::routineAt(const SourceUnit &unit, int line) {
  for (const auto &routine : unit.routines) {
    int end = routine.endLine == 0 ? static_cast<int>(unit.lines.size()) : routine.endLine;
    if (routine.startLine <= line && line <= end) {
      return &routine;
    }
  }
  return nullptr;
}

bool ModernizationAnalyzer::isStatementFunction(const std::string &code, const Routine *routine) {
  if (!routine) {
    return false;
  }
  std::smatch match;
  if (!std::regex_search(code, match, StatementFunctionRe)) {
    return false;
  }
  std::string name = lower(match[1].str());
  if (name == "if" || name == "do" || name == "call" || name == "where" || name == "forall" || name == "select") {
    return false;
  }
  if (routine->declarations.count(name) > 0) {
    return false;
  }
  return !std::regex_search(code, DeclarationRe);
}
