#pragma once

#include "Models.hpp"

#include <filesystem>

class ModernizationAnalyzer {
public:
  ProjectAnalysis analyzePath(const std::filesystem::path &path);

private:
  std::vector<SourceUnit> loadSources(const std::filesystem::path &root);
  void indexRoutines(SourceUnit &unit);
  std::vector<Finding> detectInUnit(const SourceUnit &unit);
  void computeImpact(const std::vector<SourceUnit> &files, std::vector<Finding> &findings);
  void prioritize(std::vector<Finding> &findings);

  static bool isFortranFile(const std::filesystem::path &path);
  static bool looksFixedForm(const std::vector<std::string> &lines);
  static std::string stripComment(const std::string &line);
  static std::string removeFixedLabel(const std::string &line, bool fixedForm);
  static std::string trim(const std::string &value);
  static std::string lower(std::string value);
  static std::set<std::string> declaredNames(const std::string &code);
  static const Routine *routineAt(const SourceUnit &unit, int line);
  static bool isStatementFunction(const std::string &code, const Routine *routine);
};
