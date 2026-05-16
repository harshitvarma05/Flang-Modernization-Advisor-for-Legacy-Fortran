#pragma once

#include "Findings.hpp"

#include <filesystem>
#include <string>

namespace advisor {

class FlangAstAdvisor {
public:
  ProjectAnalysis analyzePath(const std::filesystem::path &path) const;

private:
  FileAnalysis analyzeFile(const std::filesystem::path &path) const;
};

} // namespace advisor
