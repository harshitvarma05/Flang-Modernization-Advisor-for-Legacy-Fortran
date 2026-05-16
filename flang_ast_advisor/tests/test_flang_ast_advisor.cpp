#include "FlangAstAdvisor.hpp"
#include "Transform.hpp"

#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <set>
#include <string>


static std::string readFile(const std::filesystem::path &path) {
  std::ifstream input(path);
  return std::string(std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>());
}

static bool hasPattern(
    const advisor::ProjectAnalysis &analysis, const std::string &pattern) {
  for (const auto &finding : analysis.findings) {
    if (finding.pattern == pattern)
      return true;
  }
  return false;
}

int main() {
  advisor::FlangAstAdvisor analyzer;
  auto all = analyzer.analyzePath("examples/legacy/all_patterns.f");

  assert(hasPattern(all, "arithmetic-if"));
  assert(hasPattern(all, "computed-goto"));
  assert(hasPattern(all, "equivalence"));
  assert(hasPattern(all, "common"));
  assert(hasPattern(all, "implicit-typing"));
  assert(hasPattern(all, "statement-function"));
  assert(hasPattern(all, "fixed-form"));
  assert(hasPattern(all, "assumed-size-array"));
  assert(hasPattern(all, "entry"));

  auto negative = analyzer.analyzePath("examples/legacy/false_positives.f90");
  assert(!hasPattern(negative, "statement-function"));
  assert(!hasPattern(negative, "arithmetic-if"));
  assert(!hasPattern(negative, "computed-goto"));

  auto real = analyzer.analyzePath("examples/real_case_study/minpack");
  assert(!real.files.empty());
  assert(hasPattern(real, "fixed-form"));
  assert(hasPattern(real, "implicit-typing"));

  auto out = std::filesystem::temp_directory_path() / "flang-modernizer-transform-test";
  std::filesystem::remove_all(out);
  auto transformed = applySafeTransformations("examples/real_case_study/minpack/hybrd.f", out);
  assert(transformed.size() == 1);
  auto code = readFile(transformed.front());
  assert(code.find("real function wa1") == std::string::npos);
  assert(code.find("real function wa2") == std::string::npos);
  assert(code.find("real function wa3") == std::string::npos);
  assert(code.find("real function qtf") == std::string::npos);
  assert(code.find("wa3(j) = diag(j)*x(j)") != std::string::npos);

  std::cout << "In-memory Flang AST advisor tests passed.\n";
  return 0;
}
