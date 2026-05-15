CXX ?= g++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -pedantic -Iinclude
BUILD_DIR := build
CORE_SRC := src/Analyzer.cpp src/Models.cpp src/Reporter.cpp src/Transform.cpp

.PHONY: all test clean report transform

all: $(BUILD_DIR)/flang-modernizer

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/flang-modernizer: $(CORE_SRC) src/main.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(CORE_SRC) src/main.cpp -o $@

$(BUILD_DIR)/flang-modernizer-tests: $(CORE_SRC) tests/test_analyzer.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(CORE_SRC) tests/test_analyzer.cpp -o $@

test: $(BUILD_DIR)/flang-modernizer-tests
	./$(BUILD_DIR)/flang-modernizer-tests

report: $(BUILD_DIR)/flang-modernizer
	./$(BUILD_DIR)/flang-modernizer examples/case_study --output docs/case_study_report.md

transform: $(BUILD_DIR)/flang-modernizer
	./$(BUILD_DIR)/flang-modernizer examples/case_study --safe-transform-out transformed/case_study

clean:
	rm -rf $(BUILD_DIR)
