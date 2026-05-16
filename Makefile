CXX ?= c++
BUILD_DIR := build
FLANG_PREFIX ?= /opt/homebrew/opt/flang
LLVM_PREFIX ?= /opt/homebrew/opt/llvm
WARN_FLAGS ?= -Wall -Wextra -pedantic
CXXFLAGS ?= -std=c++17 $(WARN_FLAGS) -Iinclude
FLANG_CXXFLAGS := -std=c++17 $(WARN_FLAGS) -I$(FLANG_PREFIX)/include -I$(LLVM_PREFIX)/include
FLANG_LDFLAGS := -L$(FLANG_PREFIX)/lib -L$(LLVM_PREFIX)/lib -lFortranParser -lFortranSemantics -lFortranEvaluate -lFortranSupport -lFortranDecimal -lLLVM-22 -Wl,-rpath,$(FLANG_PREFIX)/lib -Wl,-rpath,$(LLVM_PREFIX)/lib
CORE_SRC := src/Analyzer.cpp src/FlangMetadata.cpp src/Models.cpp src/Reporter.cpp src/Transform.cpp
WEB_SRC := src/WebServer.cpp
AST_SRC := flang_ast_advisor/src/Findings.cpp flang_ast_advisor/src/FlangAstAdvisor.cpp

.PHONY: all test clean report transform real-report real-transform flang-validate flang-dump-tree flang-inprocess serve ast-advisor ast-test ast-report ast-real-report

all: $(BUILD_DIR)/flang-modernizer $(BUILD_DIR)/flang-ast-advisor

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/flang-modernizer: $(CORE_SRC) $(WEB_SRC) src/main.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(CORE_SRC) $(WEB_SRC) src/main.cpp -o $@

$(BUILD_DIR)/flang-modernizer-tests: $(CORE_SRC) tests/test_analyzer.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(CORE_SRC) tests/test_analyzer.cpp -o $@

$(BUILD_DIR)/flang-inprocess-probe: src/InProcessFlangProbe.cpp | $(BUILD_DIR)
	$(CXX) $(FLANG_CXXFLAGS) src/InProcessFlangProbe.cpp $(FLANG_LDFLAGS) -o $@

$(BUILD_DIR)/flang-ast-advisor: $(AST_SRC) flang_ast_advisor/src/main.cpp | $(BUILD_DIR)
	$(CXX) $(FLANG_CXXFLAGS) -Iflang_ast_advisor/include $(AST_SRC) flang_ast_advisor/src/main.cpp $(FLANG_LDFLAGS) -o $@

$(BUILD_DIR)/flang-ast-advisor-tests: $(AST_SRC) flang_ast_advisor/tests/test_flang_ast_advisor.cpp | $(BUILD_DIR)
	$(CXX) $(FLANG_CXXFLAGS) -Iflang_ast_advisor/include $(AST_SRC) flang_ast_advisor/tests/test_flang_ast_advisor.cpp $(FLANG_LDFLAGS) -o $@

test: $(BUILD_DIR)/flang-modernizer-tests $(BUILD_DIR)/flang-ast-advisor-tests
	./$(BUILD_DIR)/flang-modernizer-tests
	./$(BUILD_DIR)/flang-ast-advisor-tests

ast-advisor: $(BUILD_DIR)/flang-ast-advisor

ast-test: $(BUILD_DIR)/flang-ast-advisor-tests
	./$(BUILD_DIR)/flang-ast-advisor-tests

flang-inprocess: $(BUILD_DIR)/flang-inprocess-probe
	./$(BUILD_DIR)/flang-inprocess-probe examples/legacy/all_patterns.f

report: $(BUILD_DIR)/flang-modernizer
	./$(BUILD_DIR)/flang-modernizer examples/case_study --output docs/case_study_report.md

real-report: $(BUILD_DIR)/flang-modernizer
	./$(BUILD_DIR)/flang-modernizer examples/real_case_study/minpack --output docs/real_case_study_report.md

ast-report: $(BUILD_DIR)/flang-ast-advisor
	./$(BUILD_DIR)/flang-ast-advisor examples/case_study --output flang_ast_advisor/docs/ast_case_study_report.md --json flang_ast_advisor/docs/ast_case_study_report.json

ast-real-report: $(BUILD_DIR)/flang-ast-advisor
	./$(BUILD_DIR)/flang-ast-advisor examples/real_case_study/minpack --output flang_ast_advisor/docs/ast_real_case_study_report.md --json flang_ast_advisor/docs/ast_real_case_study_report.json

transform: $(BUILD_DIR)/flang-modernizer
	./$(BUILD_DIR)/flang-modernizer examples/case_study --safe-transform-out transformed/case_study

real-transform: $(BUILD_DIR)/flang-modernizer
	./$(BUILD_DIR)/flang-modernizer examples/real_case_study/minpack --safe-transform-out transformed/real_case_study

serve: $(BUILD_DIR)/flang-modernizer
	./$(BUILD_DIR)/flang-modernizer --serve

flang-validate:
	flang -fsyntax-only examples/legacy/all_patterns.f
	flang -fsyntax-only examples/case_study/solver.f examples/case_study/physics.f
	flang -fsyntax-only examples/real_case_study/minpack/*.f

flang-dump-tree:
	./scripts/dump_flang_tree.sh flang

clean:
	rm -rf $(BUILD_DIR)
