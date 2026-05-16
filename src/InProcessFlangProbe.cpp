#include "flang/Parser/parsing.h"
#include "flang/Parser/provenance.h"
#include "flang/Semantics/semantics.h"
#include "flang/Support/LangOptions.h"
#include "flang/Support/default-kinds.h"
#include "llvm/Support/raw_ostream.h"

#include <iostream>
#include <string>

int main(int argc, char **argv) {
  if (argc < 2) {
    std::cerr << "Usage: flang-inprocess-probe <file.f>\n";
    return 1;
  }

  Fortran::parser::AllSources allSources;
  Fortran::parser::AllCookedSources allCooked{allSources};
  Fortran::parser::Parsing parsing{allCooked};
  Fortran::parser::Options options;
  std::string path = argv[1];
  auto ext = path.size() >= 2 ? path.substr(path.size() - 2) : "";
  options.isFixedForm = ext == ".f" || ext == ".F";

  if (!parsing.Prescan(path, options)) {
    std::cerr << "Flang in-process prescan failed for " << path << "\n";
    return 2;
  }
  parsing.Parse(llvm::errs());
  if (!parsing.consumedWholeFile() || !parsing.parseTree()) {
    std::cerr << "Flang in-process parse failed for " << path << "\n";
    parsing.messages().Emit(llvm::errs(), allCooked);
    return 3;
  }

  Fortran::common::IntrinsicTypeDefaultKinds defaultKinds;
  Fortran::common::LanguageFeatureControl features;
  Fortran::common::LangOptions langOptions;
  Fortran::semantics::SemanticsContext semanticsContext{defaultKinds, features, langOptions, allCooked};
  Fortran::semantics::Semantics semantics{semanticsContext, *parsing.parseTree()};
  bool ok = semantics.Perform();

  llvm::outs() << "In-process Flang parse tree: OK\n";
  llvm::outs() << "In-process Flang semantic analysis: " << (ok && !semantics.AnyFatalError() ? "OK" : "FAILED") << "\n";
  llvm::outs() << "Semantic symbol dump follows:\n";
  semantics.DumpSymbols(llvm::outs());
  return ok && !semantics.AnyFatalError() ? 0 : 4;
}
