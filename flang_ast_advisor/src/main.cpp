#include "Findings.hpp"
#include "FlangAstAdvisor.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

int main(int argc, char **argv) {
  if (argc < 2) {
    std::cerr << "Usage: flang-ast-advisor <fortran-file-or-folder> "
                 "[--output report.md] [--json report.json]\n";
    return 1;
  }

  std::filesystem::path input = argv[1];
  std::string markdownOut;
  std::string jsonOut;
  for (int i = 2; i < argc; ++i) {
    std::string arg = argv[i];
    if (arg == "--output" && i + 1 < argc) {
      markdownOut = argv[++i];
    } else if (arg == "--json" && i + 1 < argc) {
      jsonOut = argv[++i];
    } else {
      std::cerr << "Unknown argument: " << arg << "\n";
      return 2;
    }
  }

  advisor::FlangAstAdvisor analyzer;
  auto analysis = analyzer.analyzePath(input);
  auto report = advisor::markdownReport(analysis);

  if (!markdownOut.empty()) {
    std::ofstream out(markdownOut);
    out << report;
  } else {
    std::cout << report;
  }

  if (!jsonOut.empty()) {
    std::ofstream out(jsonOut);
    out << advisor::jsonReport(analysis);
  }

  return analysis.files.empty() ? 3 : 0;
}
