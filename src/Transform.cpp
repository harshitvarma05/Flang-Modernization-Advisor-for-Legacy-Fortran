#include "Transform.hpp"

#include <algorithm>
#include <fstream>
#include <regex>
#include <set>
#include <sstream>

namespace {
std::string trim(const std::string &value) {
  auto start = value.find_first_not_of(" \t\r\n");
  if (start == std::string::npos) {
    return "";
  }
  auto end = value.find_last_not_of(" \t\r\n");
  return value.substr(start, end - start + 1);
}

std::string lower(std::string value) {
  std::transform(value.begin(), value.end(), value.begin(), [](unsigned char ch) { return std::tolower(ch); });
  return value;
}

std::vector<std::string> readLines(const std::filesystem::path &path) {
  std::ifstream input(path);
  std::vector<std::string> lines;
  std::string line;
  while (std::getline(input, line)) {
    lines.push_back(line);
  }
  return lines;
}

void writeLines(const std::filesystem::path &path, const std::vector<std::string> &lines) {
  std::ofstream output(path);
  for (const auto &line : lines) {
    output << line << "\n";
  }
}

bool isFixedSource(const std::filesystem::path &path) {
  auto ext = lower(path.extension().string());
  return ext == ".f" || ext == ".for" || ext == ".f77" || ext == ".ftn";
}

std::string rewriteFixedLine(const std::string &line) {
  if (line.empty()) {
    return line;
  }
  if (line[0] == 'C' || line[0] == 'c' || line[0] == '*') {
    return "!" + line.substr(1);
  }
  if (line.size() >= 6) {
    std::string label = trim(line.substr(0, 5));
    if (!label.empty() && label.find_first_not_of("0123456789") == std::string::npos) {
      return label + " " + line.substr(6);
    }
    if (!trim(line.substr(5, 1)).empty()) {
      return "& " + line.substr(6);
    }
  }
  return line;
}

std::set<std::string> declaredNames(const std::string &code) {
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

std::vector<std::string> internalFunctionLines(const std::string &name, const std::string &args,
                                               const std::string &expression) {
  std::vector<std::string> argNames;
  std::stringstream stream(args);
  std::string arg;
  while (std::getline(stream, arg, ',')) {
    arg = trim(arg);
    if (!arg.empty()) {
      argNames.push_back(arg);
    }
  }

  std::ostringstream signature;
  signature << "  real function " << name << "(";
  for (size_t i = 0; i < argNames.size(); ++i) {
    if (i > 0) {
      signature << ", ";
    }
    signature << argNames[i];
  }
  signature << ")";

  std::vector<std::string> lines{signature.str()};
  for (const auto &argName : argNames) {
    lines.push_back("    real, intent(in) :: " + argName);
  }
  lines.push_back("    " + name + " = " + trim(expression));
  lines.push_back("  end function " + name);
  return lines;
}

std::vector<std::string> convertSimpleStatementFunctions(const std::vector<std::string> &lines) {
  const std::regex routineRe(R"(^(program|subroutine|function)\b)", std::regex::icase);
  const std::regex endRe(R"(^end\b)", std::regex::icase);
  const std::regex declarationRe(R"(^\s*(integer|real|double\s+precision|complex|logical|character)\b)",
                                 std::regex::icase);
  const std::regex statementFunctionRe(R"(^\s*([a-z_]\w*)\s*\(([^)]*)\)\s*=\s*(.+)$)",
                                       std::regex::icase);

  std::vector<std::string> result;
  std::set<std::string> declared;
  std::vector<std::string> pendingFunctions;
  bool inRoutine = false;

  for (const auto &line : lines) {
    std::string stripped = trim(line);
    if (std::regex_search(stripped, routineRe)) {
      inRoutine = true;
      declared.clear();
      pendingFunctions.clear();
    }
    if (inRoutine && std::regex_search(stripped, declarationRe)) {
      auto names = declaredNames(stripped);
      declared.insert(names.begin(), names.end());
    }

    std::smatch match;
    if (inRoutine && std::regex_search(stripped, match, statementFunctionRe) && declared.count(lower(match[1].str())) == 0) {
      auto generated = internalFunctionLines(match[1].str(), match[2].str(), match[3].str());
      pendingFunctions.insert(pendingFunctions.end(), generated.begin(), generated.end());
      continue;
    }

    if (inRoutine && std::regex_search(stripped, endRe)) {
      if (!pendingFunctions.empty()) {
        result.push_back("contains");
        result.insert(result.end(), pendingFunctions.begin(), pendingFunctions.end());
      }
      inRoutine = false;
    }
    result.push_back(line);
  }
  return result;
}
} // namespace

std::vector<std::filesystem::path> applySafeTransformations(const std::filesystem::path &source,
                                                            const std::filesystem::path &outputDir) {
  std::filesystem::create_directories(outputDir);
  std::vector<std::filesystem::path> inputs;
  if (std::filesystem::is_regular_file(source)) {
    inputs.push_back(source);
  } else {
    for (const auto &entry : std::filesystem::recursive_directory_iterator(source)) {
      if (entry.is_regular_file() && isFixedSource(entry.path())) {
        inputs.push_back(entry.path());
      }
    }
  }
  std::sort(inputs.begin(), inputs.end());

  std::vector<std::filesystem::path> written;
  for (const auto &input : inputs) {
    auto lines = readLines(input);
    for (auto &line : lines) {
      line = rewriteFixedLine(line);
    }
    lines = convertSimpleStatementFunctions(lines);

    auto target = outputDir / input.filename();
    target.replace_extension(".f90");
    writeLines(target, lines);
    written.push_back(target);
  }
  return written;
}
