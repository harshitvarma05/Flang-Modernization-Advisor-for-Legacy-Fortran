#include "Findings.hpp"
#include "FlangAstAdvisor.hpp"
#include "Transform.hpp"
#include "WebServer.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

namespace {
void usage() {
  std::cerr << "Usage: flang-modernizer <path> [--format markdown|json] [--output file] "
               "[--safe-transform-out dir]\n"
               "       flang-modernizer --serve [--port 8080]\n";
}
} // namespace

int main(int argc, char **argv) {
  if (argc < 2) {
    usage();
    return 1;
  }

  std::string firstArg = argv[1];
  if (firstArg == "--serve") {
    int port = 8080;
    for (int i = 2; i < argc; ++i) {
      std::string arg = argv[i];
      if (arg == "--port" && i + 1 < argc) {
        port = std::stoi(argv[++i]);
      } else {
        usage();
        return 1;
      }
    }
    return runWebServer(port);
  }

  std::filesystem::path input = argv[1];
  std::string format = "markdown";
  std::filesystem::path output;
  std::filesystem::path transformOut;

  for (int i = 2; i < argc; ++i) {
    std::string arg = argv[i];
    if (arg == "--format" && i + 1 < argc) {
      format = argv[++i];
    } else if (arg == "--output" && i + 1 < argc) {
      output = argv[++i];
    } else if (arg == "--safe-transform-out" && i + 1 < argc) {
      transformOut = argv[++i];
    } else {
      usage();
      return 1;
    }
  }

  advisor::FlangAstAdvisor analyzer;
  advisor::ProjectAnalysis analysis = analyzer.analyzePath(input);
  std::string report = format == "json" ? advisor::jsonReport(analysis) : advisor::markdownReport(analysis);

  if (!output.empty()) {
    if (!output.parent_path().empty()) {
      std::filesystem::create_directories(output.parent_path());
    }
    std::ofstream file(output);
    file << report;
  } else {
    std::cout << report;
  }

  if (!transformOut.empty()) {
    auto written = applySafeTransformations(input, transformOut);
    std::cout << "\nSafe transformed files written: " << written.size() << "\n";
    for (const auto &path : written) {
      std::cout << "- " << path.string() << "\n";
    }
  }
  return 0;
}
