CXX ?= c++
BUILD_DIR := build
FLANG_PREFIX ?= /opt/homebrew/opt/flang
LLVM_PREFIX ?= /opt/homebrew/opt/llvm
CXXFLAGS ?= -std=c++17 -Wall -Wextra -pedantic -Iinclude -I$(FLANG_PREFIX)/include -I$(LLVM_PREFIX)/include
LDFLAGS ?= -L$(FLANG_PREFIX)/lib -L$(LLVM_PREFIX)/lib -lFortranParser -lFortranSemantics -lFortranEvaluate -lFortranSupport -lFortranDecimal -lLLVM-22 -Wl,-rpath,$(FLANG_PREFIX)/lib -Wl,-rpath,$(LLVM_PREFIX)/lib
CORE_SRC := src/Findings.cpp src/FlangAstAdvisor.cpp

.PHONY: all test clean report real-report

all: $(BUILD_DIR)/flang-ast-advisor

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/flang-ast-advisor: $(CORE_SRC) src/main.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(CORE_SRC) src/main.cpp $(LDFLAGS) -o $@

$(BUILD_DIR)/flang-ast-advisor-tests: $(CORE_SRC) tests/test_flang_ast_advisor.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(CORE_SRC) tests/test_flang_ast_advisor.cpp $(LDFLAGS) -o $@

test: $(BUILD_DIR)/flang-ast-advisor-tests
	cd .. && flang_ast_advisor/$(BUILD_DIR)/flang-ast-advisor-tests

report: $(BUILD_DIR)/flang-ast-advisor
	./$(BUILD_DIR)/flang-ast-advisor ../examples/case_study --output docs/ast_case_study_report.md --json docs/ast_case_study_report.json

real-report: $(BUILD_DIR)/flang-ast-advisor
	./$(BUILD_DIR)/flang-ast-advisor ../examples/real_case_study/minpack --output docs/ast_real_case_study_report.md --json docs/ast_real_case_study_report.json

clean:
	rm -rf $(BUILD_DIR)
