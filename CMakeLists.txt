cmake_minimum_required(VERSION 3.16)
project(FlangModernizationAdvisor LANGUAGES CXX)

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

set(FLANG_LIBS
  ${FORTRAN_PARSER_LIB}
  ${FORTRAN_SEMANTICS_LIB}
  ${FORTRAN_EVALUATE_LIB}
  ${FORTRAN_SUPPORT_LIB}
  ${FORTRAN_DECIMAL_LIB}
  ${LLVM_LIB}
)

add_library(safe_transform src/Transform.cpp)
target_include_directories(safe_transform PUBLIC include)

add_library(flang_ast_advisor_core
  flang_ast_advisor/src/Findings.cpp
  flang_ast_advisor/src/FlangAstAdvisor.cpp
)
target_include_directories(flang_ast_advisor_core
  PUBLIC flang_ast_advisor/include
  PRIVATE ${FLANG_INCLUDE_DIR} ${LLVM_INCLUDE_DIR}
)
target_link_libraries(flang_ast_advisor_core PUBLIC ${FLANG_LIBS})

add_executable(flang-modernizer src/main.cpp src/WebServer.cpp)
target_include_directories(flang-modernizer PRIVATE include flang_ast_advisor/include)
target_link_libraries(flang-modernizer PRIVATE flang_ast_advisor_core safe_transform)

add_executable(flang-ast-advisor flang_ast_advisor/src/main.cpp)
target_link_libraries(flang-ast-advisor PRIVATE flang_ast_advisor_core)

add_executable(flang-ast-advisor-tests flang_ast_advisor/tests/test_flang_ast_advisor.cpp)
target_include_directories(flang-ast-advisor-tests PRIVATE include)
target_link_libraries(flang-ast-advisor-tests PRIVATE flang_ast_advisor_core safe_transform)

add_executable(flang-inprocess-probe src/InProcessFlangProbe.cpp)
target_include_directories(flang-inprocess-probe PRIVATE ${FLANG_INCLUDE_DIR} ${LLVM_INCLUDE_DIR})
target_link_libraries(flang-inprocess-probe PRIVATE ${FLANG_LIBS})

add_test(NAME flang_ast_advisor_tests
  COMMAND flang-ast-advisor-tests
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

find_program(FLANG_EXECUTABLE NAMES flang flang-new REQUIRED)

add_custom_target(flang-validate
  COMMAND ${FLANG_EXECUTABLE} -fsyntax-only examples/legacy/all_patterns.f
  COMMAND ${FLANG_EXECUTABLE} -fsyntax-only examples/case_study/solver.f examples/case_study/physics.f
  COMMAND ${FLANG_EXECUTABLE} -fsyntax-only examples/real_case_study/minpack/*.f
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  COMMENT "Validating legacy Fortran fixtures and real case study with LLVM Flang"
)

add_custom_target(flang-dump-tree
  COMMAND /bin/sh scripts/dump_flang_tree.sh ${FLANG_EXECUTABLE}
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  COMMENT "Writing Flang parse-tree and symbol dumps"
)

add_custom_target(flang-inprocess
  COMMAND flang-inprocess-probe examples/legacy/all_patterns.f
  DEPENDS flang-inprocess-probe
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  COMMENT "Running in-process Flang parser and semantic analysis probe"
)

add_custom_target(report
  COMMAND flang-modernizer examples/case_study --output docs/case_study_report.md
  DEPENDS flang-modernizer
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  COMMENT "Generating AST case-study report"
)

add_custom_target(real-report
  COMMAND flang-modernizer examples/real_case_study/minpack --output docs/real_case_study_report.md
  DEPENDS flang-modernizer
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  COMMENT "Generating AST real MINPACK report"
)

add_custom_target(ast-report
  COMMAND flang-ast-advisor examples/case_study --output flang_ast_advisor/docs/ast_case_study_report.md --json flang_ast_advisor/docs/ast_case_study_report.json
  DEPENDS flang-ast-advisor
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  COMMENT "Generating in-memory Flang AST case-study report"
)

add_custom_target(ast-real-report
  COMMAND flang-ast-advisor examples/real_case_study/minpack --output flang_ast_advisor/docs/ast_real_case_study_report.md --json flang_ast_advisor/docs/ast_real_case_study_report.json
  DEPENDS flang-ast-advisor
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  COMMENT "Generating in-memory Flang AST real-codebase report"
)

add_custom_target(transform
  COMMAND flang-modernizer examples/case_study --safe-transform-out transformed/case_study
  DEPENDS flang-modernizer
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  COMMENT "Generating safe transformed case-study files"
)

add_custom_target(real-transform
  COMMAND flang-modernizer examples/real_case_study/minpack --safe-transform-out transformed/real_case_study
  DEPENDS flang-modernizer
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  COMMENT "Generating safe transformed files for real MINPACK case study"
)

add_custom_target(serve
  COMMAND flang-modernizer --serve
  DEPENDS flang-modernizer
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  COMMENT "Starting local web dashboard at http://localhost:8080"
)
