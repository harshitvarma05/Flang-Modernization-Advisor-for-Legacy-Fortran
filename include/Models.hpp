#pragma once

#include <set>
#include <string>
#include <vector>

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
  Location location;
  std::string construct;
  std::string routine;
  Effort effort = Effort::Moderate;
  Safety safety = Safety::ReviewNeeded;
  std::set<std::string> affectedFiles;
  std::vector<std::string> dependentConstructs;
  std::vector<std::string> behaviorRisks;
  int priority = 50;
};

struct Routine {
  std::string name;
  std::string kind;
  std::string file;
  int startLine = 1;
  int endLine = 0;
  bool hasImplicitNone = false;
  std::set<std::string> declarations;
  std::set<std::string> commonBlocks;
};

struct SourceUnit {
  std::string path;
  std::vector<std::string> lines;
  bool fixedForm = false;
  std::vector<Routine> routines;
};

struct ProjectAnalysis {
  std::string root;
  std::vector<SourceUnit> files;
  std::vector<Finding> findings;
};

std::string toString(Effort effort);
std::string toString(Safety safety);
