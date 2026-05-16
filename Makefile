CXX ?= g++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -pedantic -Iinclude
BUILD_DIR := build
FLANG_PREFIX ?= /opt/homebrew/opt/flang
LLVM_PREFIX ?= /opt/homebrew/opt/llvm
CORE_SRC := src/Analyzer.cpp src/FlangMetadata.cpp src/Models.cpp src/Reporter.cpp src/Transform.cpp
WEB_SRC := src/WebServer.cpp

.PHONY: all test clean report transform real-report real-transform flang-validate flang-dump-tree flang-inprocess serve ast-advisor ast-test ast-report ast-real-report

all: $(BUILD_DIR)/flang-modernizer

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/flang-modernizer: $(CORE_SRC) $(WEB_SRC) src/main.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(CORE_SRC) $(WEB_SRC) src/main.cpp -o $@

$(BUILD_DIR)/flang-modernizer-tests: $(CORE_SRC) tests/test_analyzer.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(CORE_SRC) tests/test_analyzer.cpp -o $@

$(BUILD_DIR)/flang-inprocess-probe: src/InProcessFlangProbe.cpp | $(BUILD_DIR)
	$(CXX) -std=c++17 -I$(FLANG_PREFIX)/include -I$(LLVM_PREFIX)/include src/InProcessFlangProbe.cpp -L$(FLANG_PREFIX)/lib -L$(LLVM_PREFIX)/lib -lFortranParser -lFortranSemantics -lFortranEvaluate -lFortranSupport -lFortranDecimal -lLLVM-22 -Wl,-rpath,$(FLANG_PREFIX)/lib -Wl,-rpath,$(LLVM_PREFIX)/lib -o $@

test: $(BUILD_DIR)/flang-modernizer-tests
	./$(BUILD_DIR)/flang-modernizer-tests

flang-inprocess: $(BUILD_DIR)/flang-inprocess-probe
	./$(BUILD_DIR)/flang-inprocess-probe examples/legacy/all_patterns.f

report: $(BUILD_DIR)/flang-modernizer
	./$(BUILD_DIR)/flang-modernizer examples/case_study --output docs/case_study_report.md

real-report: $(BUILD_DIR)/flang-modernizer
	./$(BUILD_DIR)/flang-modernizer examples/real_case_study/minpack --output docs/real_case_study_report.md

transform: $(BUILD_DIR)/flang-modernizer
	./$(BUILD_DIR)/flang-modernizer examples/case_study --safe-transform-out transformed/case_study

real-transform: $(BUILD_DIR)/flang-modernizer
	./$(BUILD_DIR)/flang-modernizer examples/real_case_study/minpack --safe-transform-out transformed/real_case_study

serve: $(BUILD_DIR)/flang-modernizer
	./$(BUILD_DIR)/flang-modernizer --serve

flang-validate:
	flang -fsyntax-only examples/legacy/all_patterns.f
	flang -fsyntax-only examples/case_study/solver.f examples/case_study/physics.f

flang-dump-tree:
	./scripts/dump_flang_tree.sh flang

clean:
	rm -rf $(BUILD_DIR)


ast-advisor:
	$(MAKE) -C flang_ast_advisor

ast-test:
	$(MAKE) -C flang_ast_advisor test

ast-report:
	$(MAKE) -C flang_ast_advisor report

ast-real-report:
	$(MAKE) -C flang_ast_advisor real-report
