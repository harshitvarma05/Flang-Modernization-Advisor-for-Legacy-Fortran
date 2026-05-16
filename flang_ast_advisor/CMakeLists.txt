cmake_minimum_required(VERSION 3.16)
project(InMemoryFlangAstAdvisor LANGUAGES CXX)

include(CTest)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_path(FLANG_INCLUDE_DIR flang/Parser/parsing.h HINTS /opt/homebrew/opt/flang/include)
find_path(LLVM_INCLUDE_DIR llvm/Support/raw_ostream.h HINTS /opt/homebrew/opt/llvm/include)
find_library(FORTRAN_PARSER_LIB FortranParser HINTS /opt/homebrew/opt/flang/lib)
find_library(FORTRAN_SEMANTICS_LIB FortranSemantics HINTS /opt/homebrew/opt/flang/lib)
find_library(FORTRAN_EVALUATE_LIB FortranEvaluate HINTS /opt/homebrew/opt/flang/lib)
find_library(FORTRAN_SUPPORT_LIB FortranSupport HINTS /opt/homebrew/opt/flang/lib)
find_library(FORTRAN_DECIMAL_LIB FortranDecimal HINTS /opt/homebrew/opt/flang/lib)
find_library(LLVM_LIB LLVM-22 HINTS /opt/homebrew/opt/llvm/lib)

if(NOT FLANG_INCLUDE_DIR OR NOT LLVM_INCLUDE_DIR OR NOT FORTRAN_PARSER_LIB OR
   NOT FORTRAN_SEMANTICS_LIB OR NOT FORTRAN_EVALUATE_LIB OR
   NOT FORTRAN_SUPPORT_LIB OR NOT FORTRAN_DECIMAL_LIB OR NOT LLVM_LIB)
  message(FATAL_ERROR "Flang/LLVM development headers and libraries were not found. Install Homebrew flang and llvm.")
endif()

add_library(flang_ast_advisor_core
  src/Findings.cpp
  src/FlangAstAdvisor.cpp
)
target_include_directories(flang_ast_advisor_core
  PUBLIC include
  PRIVATE ${FLANG_INCLUDE_DIR} ${LLVM_INCLUDE_DIR}
)
target_link_libraries(flang_ast_advisor_core
  PUBLIC
    ${FORTRAN_PARSER_LIB}
    ${FORTRAN_SEMANTICS_LIB}
    ${FORTRAN_EVALUATE_LIB}
    ${FORTRAN_SUPPORT_LIB}
    ${FORTRAN_DECIMAL_LIB}
    ${LLVM_LIB}
)

add_executable(flang-ast-advisor src/main.cpp)
target_link_libraries(flang-ast-advisor PRIVATE flang_ast_advisor_core)

add_executable(flang-ast-advisor-tests tests/test_flang_ast_advisor.cpp)
target_link_libraries(flang-ast-advisor-tests PRIVATE flang_ast_advisor_core)

add_test(NAME flang_ast_advisor_tests
  COMMAND flang-ast-advisor-tests
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/..
)

add_custom_target(ast-report
  COMMAND flang-ast-advisor ../examples/case_study --output docs/ast_case_study_report.md --json docs/ast_case_study_report.json
  DEPENDS flang-ast-advisor
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  COMMENT "Generating in-memory Flang AST case-study report"
)

add_custom_target(ast-real-report
  COMMAND flang-ast-advisor ../examples/real_case_study/minpack --output docs/ast_real_case_study_report.md --json docs/ast_real_case_study_report.json
  DEPENDS flang-ast-advisor
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  COMMENT "Generating in-memory Flang AST real-codebase report"
)
