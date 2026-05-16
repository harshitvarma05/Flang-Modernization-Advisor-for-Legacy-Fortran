#pragma once

#include <map>
#include <set>
#include <string>
#include <vector>

namespace advisor {

enum class Effort { Trivial, Moderate, Complex };
enum class Safety { Safe, ReviewNeeded, Risky };

struct Location {
  std::string file;
  int line = 1;
  int column = 1;
};

struct Finding {
  std::string pattern;
  std::string message;
  std::string construct;
  std::string routine;
  Location location;
  Effort effort = Effort::Moderate;
  Safety safety = Safety::ReviewNeeded;
  std::set<std::string> affectedFiles;
  std::vector<std::string> dependentConstructs;
  std::vector<std::string> behaviorRisks;
  std::vector<std::string> flangEvidence;
  std::vector<std::string> semanticEvidence;
  int priority = 50;
};

struct FileAnalysis {
  std::string path;
  bool fixedForm = false;
  bool parsed = false;
  bool semanticsOk = false;
  std::vector<Finding> findings;
};

struct ProjectAnalysis {
  std::string root;
  std::vector<FileAnalysis> files;
  std::vector<Finding> findings;
};

std::string toString(Effort effort);
std::string toString(Safety safety);
void prioritize(ProjectAnalysis &analysis);
std::string markdownReport(const ProjectAnalysis &analysis);
std::string jsonReport(const ProjectAnalysis &analysis);

} // namespace advisor
