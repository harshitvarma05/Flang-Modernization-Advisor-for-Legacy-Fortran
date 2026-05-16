#include "FlangAstAdvisor.hpp"

#include "flang/Common/visit.h"
#include "flang/Parser/parse-tree-visitor.h"
#include "flang/Parser/parsing.h"
#include "flang/Parser/provenance.h"
#include "flang/Semantics/scope.h"
#include "flang/Semantics/semantics.h"
#include "flang/Semantics/symbol.h"
#include "flang/Support/LangOptions.h"
#include "flang/Support/default-kinds.h"
#include "llvm/Support/raw_ostream.h"

#include <algorithm>
#include <filesystem>
#include <map>
#include <optional>
#include <sstream>

namespace advisor {
namespace fs = std::filesystem;
namespace parser = Fortran::parser;
namespace semantics = Fortran::semantics;

static bool isFortranFile(const fs::path &path) {
  auto ext = path.extension().string();
  std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
  return ext == ".f" || ext == ".for" || ext == ".f77" || ext == ".f90" ||
      ext == ".f95" || ext == ".f03" || ext == ".f08";
}

static bool isFixedFormFile(const fs::path &path) {
  auto ext = path.extension().string();
  std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
  return ext == ".f" || ext == ".for" || ext == ".f77";
}

static std::string labelString(Fortran::common::Label label) {
  return std::to_string(static_cast<unsigned long long>(label));
}

static std::string nameString(const parser::Name &name) {
  return name.source.ToString();
}

static Location locationFrom(
    const parser::AllCookedSources &cooked, parser::CharBlock source,
    const std::string &fallbackFile) {
  Location loc;
  loc.file = fallbackFile;
  if (auto range = cooked.GetSourcePositionRange(source)) {
    loc.file = *range->first.path;
    loc.line = range->first.line;
    loc.column = range->first.column;
  }
  return loc;
}

static std::string blockName(const std::optional<parser::Name> &name) {
  if (!name)
    return "blank-common";
  return "/" + nameString(*name) + "/";
}

static std::string join(const std::vector<std::string> &items,
    const std::string &sep = ", ") {
  std::ostringstream out;
  for (std::size_t i = 0; i < items.size(); ++i) {
    if (i)
      out << sep;
    out << items[i];
  }
  return out.str();
}

class AstFindingVisitor {
public:
  AstFindingVisitor(const parser::AllCookedSources &cooked, std::string file,
      bool fixedForm, std::vector<Finding> &findings)
      : cooked_{cooked}, file_{std::move(file)}, fixedForm_{fixedForm},
        findings_{findings} {}

  template <typename T> bool Pre(const T &) { return true; }
  template <typename T> void Post(const T &) {}

  template <typename T> bool Pre(const parser::Statement<T> &stmt) {
    statementSources_.push_back(stmt.source);
    return true;
  }

  template <typename T> void Post(const parser::Statement<T> &) {
    if (!statementSources_.empty())
      statementSources_.pop_back();
  }

  bool Pre(const parser::MainProgram &program) {
    routineStack_.push_back(mainProgramName(program));
    return true;
  }
  void Post(const parser::MainProgram &) { popRoutine(); }

  bool Pre(const parser::FunctionSubprogram &program) {
    const auto &stmt = std::get<0>(program.t).statement;
    routineStack_.push_back(nameString(std::get<1>(stmt.t)));
    return true;
  }
  void Post(const parser::FunctionSubprogram &) { popRoutine(); }

  bool Pre(const parser::SubroutineSubprogram &program) {
    const auto &stmt = std::get<0>(program.t).statement;
    routineStack_.push_back(nameString(std::get<1>(stmt.t)));
    return true;
  }
  void Post(const parser::SubroutineSubprogram &) { popRoutine(); }

  bool Pre(const parser::ImplicitStmt &stmt) {
    if (std::holds_alternative<std::list<parser::ImplicitSpec>>(stmt.u)) {
      Finding f = make("implicit-typing", currentStatementSource(),
          "Replace implicit type maps with IMPLICIT NONE plus explicit declarations.",
          "IMPLICIT type map");
      f.effort = Effort::Moderate;
      f.safety = Safety::ReviewNeeded;
      f.dependentConstructs.push_back("Flang-resolved implicit declaration rules");
      f.behaviorRisks.push_back(
          "Adding IMPLICIT NONE can expose undeclared symbols and misspellings.");
      f.flangEvidence.push_back(
          "Visited parser::ImplicitStmt containing implicit-spec-list.");
      findings_.push_back(std::move(f));
    }
    return true;
  }

  bool Pre(const parser::ArithmeticIfStmt &stmt) {
    const auto &labels = std::make_tuple(std::get<1>(stmt.t), std::get<2>(stmt.t),
        std::get<3>(stmt.t));
    std::vector<std::string> targets{labelString(std::get<0>(labels)),
        labelString(std::get<1>(labels)), labelString(std::get<2>(labels))};
    Finding f = make("arithmetic-if", currentStatementSource(),
        "Replace arithmetic IF with structured IF/ELSE blocks.",
        currentStatementText());
    f.effort = Effort::Moderate;
    f.safety = Safety::ReviewNeeded;
    f.dependentConstructs.push_back("target labels: " + join(targets));
    f.behaviorRisks.push_back(
        "Branch ordering depends on negative, zero, and positive expression values.");
    f.flangEvidence.push_back("Visited parser::ArithmeticIfStmt in memory.");
    findings_.push_back(std::move(f));
    return true;
  }

  bool Pre(const parser::ComputedGotoStmt &stmt) {
    std::vector<std::string> targets;
    for (auto label : std::get<0>(stmt.t))
      targets.push_back(labelString(label));
    Finding f = make("computed-goto", currentStatementSource(),
        "Replace computed GOTO with SELECT CASE or a dispatch table.",
        currentStatementText());
    f.effort = Effort::Moderate;
    f.safety = Safety::ReviewNeeded;
    f.dependentConstructs.push_back("target labels: " + join(targets));
    f.behaviorRisks.push_back(
        "Index expressions outside the label range preserve fall-through behavior.");
    f.flangEvidence.push_back("Visited parser::ComputedGotoStmt in memory.");
    findings_.push_back(std::move(f));
    return true;
  }

  bool Pre(const parser::CommonStmt &stmt) {
    for (const auto &block : stmt.v) {
      const auto &name = std::get<0>(block.t);
      const auto &objects = std::get<1>(block.t);
      std::vector<std::string> objectNames;
      for (const auto &object : objects)
        objectNames.push_back(nameString(std::get<0>(object.t)));
      Finding f = make("common", currentStatementSource(),
          "Replace COMMON storage with a module after checking layout compatibility.",
          "COMMON " + blockName(name));
      f.effort = Effort::Complex;
      f.safety = Safety::Risky;
      f.dependentConstructs.push_back("COMMON objects: " + join(objectNames));
      f.behaviorRisks.push_back(
          "Changing storage association can alter aliasing, initialization, and binary I/O layout.");
      f.flangEvidence.push_back("Visited parser::CommonStmt in memory.");
      findings_.push_back(std::move(f));
    }
    return true;
  }

  bool Pre(const parser::EquivalenceStmt &stmt) {
    int setNo = 0;
    for (const auto &set : stmt.v) {
      ++setNo;
      std::vector<std::string> objects;
      for (const auto &object : set)
        objects.push_back(object.v.value().source.ToString());
      Finding f = make("equivalence", currentStatementSource(),
          "Review EQUIVALENCE manually before replacing storage overlays.",
          "EQUIVALENCE set " + std::to_string(setNo));
      f.effort = Effort::Complex;
      f.safety = Safety::Risky;
      f.dependentConstructs.push_back("overlaid objects: " + join(objects));
      f.behaviorRisks.push_back(
          "Removing this overlay can change aliasing, alignment, and old binary layout assumptions.");
      f.flangEvidence.push_back("Visited parser::EquivalenceStmt in memory.");
      findings_.push_back(std::move(f));
    }
    return true;
  }

  bool Pre(const parser::StmtFunctionStmt &stmt) {
    std::vector<std::string> args;
    for (const auto &arg : std::get<1>(stmt.t))
      args.push_back(nameString(arg));
    Finding f = make("statement-function", currentStatementSource(),
        "Convert statement function to an internal function.",
        nameString(std::get<0>(stmt.t)) + "(" + join(args) + ")");
    f.effort = Effort::Moderate;
    f.safety = Safety::Safe;
    f.dependentConstructs.push_back("dummy arguments: " + join(args));
    f.flangEvidence.push_back("Visited parser::StmtFunctionStmt in memory.");
    findings_.push_back(std::move(f));
    return true;
  }

  bool Pre(const parser::EntryStmt &stmt) {
    Finding f = make("entry", currentStatementSource(),
        "Split ENTRY alternate entry point into explicit procedures after call-site review.",
        "ENTRY " + nameString(std::get<0>(stmt.t)));
    f.effort = Effort::Complex;
    f.safety = Safety::Risky;
    f.dependentConstructs.push_back("alternate entry point: " +
        nameString(std::get<0>(stmt.t)));
    f.behaviorRisks.push_back(
        "ENTRY can share local variable lifetime and initialization with the host procedure.");
    f.flangEvidence.push_back("Visited parser::EntryStmt in memory.");
    findings_.push_back(std::move(f));
    return true;
  }

  bool Pre(const parser::AssumedImpliedSpec &stmt) {
    Finding f = make("assumed-size-array", currentStatementSource(),
        "Consider assumed-shape arrays only after introducing explicit interfaces.",
        currentStatementText());
    f.effort = Effort::Moderate;
    f.safety = Safety::ReviewNeeded;
    f.dependentConstructs.push_back("dummy array bound uses assumed-size '*'");
    f.behaviorRisks.push_back(
        "Changing to assumed-shape changes calling conventions and requires explicit interfaces.");
    f.flangEvidence.push_back("Visited parser::AssumedImpliedSpec in memory; Flang represents REAL A(*) as an implied-shape-or-assumed-size spec.");
    findings_.push_back(std::move(f));
    return true;
  }

  void addFixedFormFinding() {
    if (!fixedForm_)
      return;
    Finding f;
    f.pattern = "fixed-form";
    f.message = "Convert fixed-form source to free-form Fortran.";
    f.construct = "source form";
    f.routine = currentRoutine();
    f.location = Location{file_, 1, 1};
    f.effort = Effort::Trivial;
    f.safety = Safety::Safe;
    f.affectedFiles.insert(file_);
    f.dependentConstructs.push_back("Flang prescan configured as fixed-form");
    f.flangEvidence.push_back(
        "File was parsed in memory with parser::Options::isFixedForm=true.");
    findings_.push_back(std::move(f));
  }

private:
  Finding make(const std::string &pattern, parser::CharBlock source,
      const std::string &message, const std::string &construct) const {
    Finding f;
    f.pattern = pattern;
    f.message = message;
    f.construct = construct;
    f.routine = currentRoutine();
    f.location = locationFrom(cooked_, source, file_);
    f.affectedFiles.insert(f.location.file);
    return f;
  }

  std::string mainProgramName(const parser::MainProgram &program) const {
    const auto &maybeStmt = std::get<0>(program.t);
    if (!maybeStmt)
      return "main-program";
    return nameString(maybeStmt->statement.v);
  }

  std::string currentRoutine() const {
    if (routineStack_.empty())
      return "";
    return routineStack_.back();
  }

  void popRoutine() {
    if (!routineStack_.empty())
      routineStack_.pop_back();
  }

  parser::CharBlock currentStatementSource() const {
    if (statementSources_.empty())
      return parser::CharBlock{};
    return statementSources_.back();
  }

  std::string currentStatementText() const {
    auto source = currentStatementSource();
    auto text = source.ToString();
    return text.empty() ? "<flang AST node>" : text;
  }

  const parser::AllCookedSources &cooked_;
  std::string file_;
  bool fixedForm_;
  std::vector<Finding> &findings_;
  std::vector<std::string> routineStack_;
  std::vector<parser::CharBlock> statementSources_;
};

static std::string scopeName(const semantics::Scope &scope) {
  if (auto name = scope.GetName())
    return name->ToString();
  switch (scope.kind()) {
  case semantics::Scope::Kind::Global:
    return "global";
  case semantics::Scope::Kind::MainProgram:
    return "main-program";
  case semantics::Scope::Kind::Subprogram:
    return "subprogram";
  case semantics::Scope::Kind::Module:
    return "module";
  default:
    return "scope";
  }
}

static Location locationFromSourceName(
    const parser::AllCookedSources &cooked, Fortran::semantics::SourceName name,
    const std::string &fallbackFile) {
  return locationFrom(cooked, name, fallbackFile);
}

static void collectSemanticScopeEvidence(const semantics::Scope &scope,
    const parser::AllCookedSources &cooked, const std::string &file,
    std::vector<Finding> &findings) {
  for (const auto &[name, symbolRef] : scope.commonBlocks()) {
    const auto &symbol = *symbolRef;
    const auto *details = symbol.detailsIf<semantics::CommonBlockDetails>();
    if (!details)
      continue;
    std::vector<std::string> objects;
    for (const auto &objectRef : details->objects()) {
      const auto &object = *objectRef;
      std::ostringstream item;
      item << object.name().ToString() << "@offset=" << object.offset()
           << ",size=" << object.size();
      objects.push_back(item.str());
    }
    Finding f;
    f.pattern = "common";
    f.message =
        "Flang semantic scope contains a COMMON block; compare layouts before module conversion.";
    f.construct = "COMMON /" + name.ToString() + "/";
    f.routine = scopeName(scope);
    f.location = locationFromSourceName(cooked, symbol.name(), file);
    f.effort = Effort::Complex;
    f.safety = Safety::Risky;
    f.affectedFiles.insert(file);
    f.dependentConstructs.push_back("semantic COMMON members: " + join(objects));
    f.behaviorRisks.push_back(
        "Semantic storage offsets show layout-sensitive modernization.");
    f.semanticEvidence.push_back(
        "Read semantics::Scope::commonBlocks() and CommonBlockDetails::objects() in memory.");
    findings.push_back(std::move(f));
  }

  int eqSetNo = 0;
  for (const auto &set : scope.equivalenceSets()) {
    ++eqSetNo;
    std::vector<std::string> objects;
    for (const auto &object : set) {
      std::ostringstream item;
      item << object.AsFortran();
      objects.push_back(item.str());
    }
    Finding f;
    f.pattern = "equivalence";
    f.message =
        "Flang semantic scope resolved an EQUIVALENCE set; keep as risky until aliasing is reviewed.";
    f.construct = "semantic EQUIVALENCE set " + std::to_string(eqSetNo);
    f.routine = scopeName(scope);
    f.location = Location{file, 1, 1};
    f.effort = Effort::Complex;
    f.safety = Safety::Risky;
    f.affectedFiles.insert(file);
    f.dependentConstructs.push_back("semantic aliases: " + join(objects));
    f.behaviorRisks.push_back(
        "Flang resolved actual storage association, so this is a real aliasing dependency.");
    f.semanticEvidence.push_back(
        "Read semantics::Scope::equivalenceSets() in memory.");
    findings.push_back(std::move(f));
  }

  bool emittedImplicit = false;
  std::vector<std::string> implicitSymbols;
  for (const auto &[name, symbolRef] : scope) {
    const auto &symbol = *symbolRef;
    if (symbol.test(semantics::Symbol::Flag::Implicit)) {
      implicitSymbols.push_back(name.ToString());
      emittedImplicit = true;
    }
  }
  if (emittedImplicit) {
    Finding f;
    f.pattern = "implicit-typing";
    f.message =
        "Flang semantics resolved implicitly typed symbols; add explicit declarations before IMPLICIT NONE.";
    f.construct = "implicit symbols";
    f.routine = scopeName(scope);
    f.location = Location{file, 1, 1};
    f.effort = Effort::Moderate;
    f.safety = Safety::ReviewNeeded;
    f.affectedFiles.insert(file);
    f.dependentConstructs.push_back("implicit symbols: " + join(implicitSymbols));
    f.behaviorRisks.push_back(
        "Modernization can fail compilation until every implicit symbol has an explicit declaration.");
    f.semanticEvidence.push_back(
        "Read semantics::Symbol::Flag::Implicit from the in-memory symbol table.");
    findings.push_back(std::move(f));
  }

  for (const auto &child : scope.children())
    collectSemanticScopeEvidence(child, cooked, file, findings);
}

FileAnalysis FlangAstAdvisor::analyzeFile(const fs::path &path) const {
  FileAnalysis result;
  result.path = path.string();
  result.fixedForm = isFixedFormFile(path);

  parser::AllSources allSources;
  parser::AllCookedSources allCooked{allSources};
  parser::Parsing parsing{allCooked};
  parser::Options options;
  options.isFixedForm = result.fixedForm;

  if (!parsing.Prescan(path.string(), options)) {
    Finding f;
    f.pattern = "parse-error";
    f.message = "Flang in-process prescan failed.";
    f.location = Location{path.string(), 1, 1};
    result.findings.push_back(std::move(f));
    return result;
  }

  parsing.Parse(llvm::errs());
  if (!parsing.consumedWholeFile() || !parsing.parseTree()) {
    Finding f;
    f.pattern = "parse-error";
    f.message = "Flang in-process parse failed.";
    f.location = Location{path.string(), 1, 1};
    result.findings.push_back(std::move(f));
    return result;
  }
  result.parsed = true;

  AstFindingVisitor visitor{allCooked, path.string(), result.fixedForm,
      result.findings};
  visitor.addFixedFormFinding();
  parser::Walk(*parsing.parseTree(), visitor);

  Fortran::common::IntrinsicTypeDefaultKinds defaultKinds;
  Fortran::common::LanguageFeatureControl features;
  Fortran::common::LangOptions langOptions;
  semantics::SemanticsContext semanticsContext{
      defaultKinds, features, langOptions, allCooked};
  semantics::Semantics semantics{semanticsContext, *parsing.parseTree()};
  result.semanticsOk = semantics.Perform() && !semantics.AnyFatalError();
  collectSemanticScopeEvidence(
      semanticsContext.globalScope(), allCooked, path.string(), result.findings);

  return result;
}

ProjectAnalysis FlangAstAdvisor::analyzePath(const fs::path &path) const {
  ProjectAnalysis project;
  project.root = path.string();

  std::vector<fs::path> files;
  if (fs::is_regular_file(path) && isFortranFile(path)) {
    files.push_back(path);
  } else if (fs::is_directory(path)) {
    for (const auto &entry : fs::recursive_directory_iterator(path)) {
      if (entry.is_regular_file() && isFortranFile(entry.path()))
        files.push_back(entry.path());
    }
  }
  std::sort(files.begin(), files.end());

  for (const auto &file : files) {
    auto fileAnalysis = analyzeFile(file);
    for (auto &finding : fileAnalysis.findings)
      project.findings.push_back(finding);
    project.files.push_back(std::move(fileAnalysis));
  }

  std::map<std::string, std::set<std::string>> commonUsers;
  for (const auto &finding : project.findings) {
    if (finding.pattern == "common")
      commonUsers[finding.construct].insert(finding.location.file);
  }
  for (auto &finding : project.findings) {
    if (finding.pattern != "common")
      continue;
    auto it = commonUsers.find(finding.construct);
    if (it == commonUsers.end())
      continue;
    finding.affectedFiles.insert(it->second.begin(), it->second.end());
    if (it->second.size() > 1) {
      finding.dependentConstructs.push_back(
          "cross-file COMMON users: " + std::to_string(it->second.size()));
      finding.behaviorRisks.push_back(
          "Module replacement requires updating every semantic COMMON declaration consistently.");
    }
  }

  prioritize(project);
  return project;
}

} // namespace advisor
