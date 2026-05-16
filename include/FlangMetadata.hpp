#pragma once

#include "Models.hpp"

#include <map>
#include <set>
#include <string>
#include <vector>

struct FlangFileMetadata {
  bool parseTreeAvailable = false;
  bool symbolsAvailable = false;
  std::set<std::string> parseConstructs;
  std::vector<std::string> symbolEvidence;
};

class FlangMetadataProvider {
public:
  std::map<std::string, FlangFileMetadata> collect(const std::vector<SourceUnit> &files) const;

private:
  static std::string run(const std::string &command, int &status);
  static std::string quote(const std::string &value);
  static FlangFileMetadata parseOutputs(const std::string &parseTree, const std::string &symbols);
};
