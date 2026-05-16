#include "FlangMetadata.hpp"

#include <array>
#include <cstdio>
#include <regex>
#include <sstream>

std::map<std::string, FlangFileMetadata> FlangMetadataProvider::collect(const std::vector<SourceUnit> &files) const {
  std::map<std::string, FlangFileMetadata> result;
  for (const auto &unit : files) {
    int parseStatus = 0;
    int symbolStatus = 0;
    std::string parseTree = run("flang -fc1 -fdebug-dump-parse-tree " + quote(unit.path), parseStatus);
    std::string symbols = run("flang -fc1 -fdebug-dump-symbols " + quote(unit.path), symbolStatus);
    auto metadata = parseOutputs(parseStatus == 0 ? parseTree : "", symbolStatus == 0 ? symbols : "");
    metadata.parseTreeAvailable = parseStatus == 0;
    metadata.symbolsAvailable = symbolStatus == 0;
    result[unit.path] = std::move(metadata);
  }
  return result;
}

std::string FlangMetadataProvider::run(const std::string &command, int &status) {
  std::array<char, 512> buffer{};
  std::string output;
  FILE *pipe = popen((command + " 2>&1").c_str(), "r");
  if (!pipe) {
    status = -1;
    return "";
  }
  while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
    output += buffer.data();
  }
  status = pclose(pipe);
  return output;
}

std::string FlangMetadataProvider::quote(const std::string &value) {
  std::string out = "'";
  for (char ch : value) {
    out += ch == '\'' ? "'\\''" : std::string(1, ch);
  }
  out += "'";
  return out;
}

FlangFileMetadata FlangMetadataProvider::parseOutputs(const std::string &parseTree, const std::string &symbols) {
  FlangFileMetadata metadata;
  const std::vector<std::pair<std::string, std::string>> constructMap{
      {"ArithmeticIfStmt", "arithmetic-if"},       {"ComputedGotoStmt", "computed-goto"},
      {"CommonStmt", "common-block"},             {"EquivalenceStmt", "equivalence"},
      {"StmtFunctionStmt", "statement-function"}, {"EntryStmt", "entry"},
      {"ArraySpec -> ImpliedShapeSpec -> AssumedImpliedSpec", "assumed-size-array"}};
  for (const auto &[needle, pattern] : constructMap) {
    if (parseTree.find(needle) != std::string::npos) {
      metadata.parseConstructs.insert(pattern);
    }
  }

  std::istringstream symbolStream(symbols);
  std::string line;
  std::regex commonRe(R"(^\s*([A-Za-z_]\w*)\s+size=.*CommonBlockDetails.*: (.*)$)");
  std::regex equivRe(R"(^\s*Equivalence Sets: (.*)$)");
  std::regex assumedSizeRe(R"(^\s*([A-Za-z_]\w*).*ObjectEntity dummy type:.*shape:.*\*)");
  std::regex stmtFunctionRe(R"(^\s*([A-Za-z_]\w*).*\(Function, StmtFunction\))");
  std::smatch match;
  while (std::getline(symbolStream, line)) {
    if (std::regex_search(line, match, commonRe)) {
      metadata.symbolEvidence.push_back("Flang symbols: COMMON /" + match[1].str() + "/ layout => " + match[2].str());
    } else if (std::regex_search(line, match, equivRe)) {
      metadata.symbolEvidence.push_back("Flang symbols: equivalence set => " + match[1].str());
    } else if (std::regex_search(line, match, assumedSizeRe)) {
      metadata.symbolEvidence.push_back("Flang symbols: assumed-size dummy => " + match[1].str());
    } else if (std::regex_search(line, match, stmtFunctionRe)) {
      metadata.symbolEvidence.push_back("Flang symbols: statement function => " + match[1].str());
    }
  }
  return metadata;
}
