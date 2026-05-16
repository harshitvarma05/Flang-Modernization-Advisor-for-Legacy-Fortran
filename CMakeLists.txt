cmake_minimum_required(VERSION 3.16)
project(FlangModernizationAdvisor LANGUAGES CXX)

include(CTest)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(flang_modernizer_core
  src/Analyzer.cpp
  src/FlangMetadata.cpp
  src/Models.cpp
  src/Reporter.cpp
  src/Transform.cpp
)
target_include_directories(flang_modernizer_core PUBLIC include)

add_executable(flang-modernizer src/main.cpp src/WebServer.cpp)
target_link_libraries(flang-modernizer PRIVATE flang_modernizer_core)

add_executable(flang-modernizer-tests tests/test_analyzer.cpp)
target_link_libraries(flang-modernizer-tests PRIVATE flang_modernizer_core)

find_path(FLANG_INCLUDE_DIR flang/Parser/parsing.h HINTS /opt/homebrew/opt/flang/include)
find_path(LLVM_INCLUDE_DIR llvm/Support/raw_ostream.h HINTS /opt/homebrew/opt/llvm/include)
find_library(FORTRAN_PARSER_LIB FortranParser HINTS /opt/homebrew/opt/flang/lib)
find_library(FORTRAN_SEMANTICS_LIB FortranSemantics HINTS /opt/homebrew/opt/flang/lib)
find_library(FORTRAN_EVALUATE_LIB FortranEvaluate HINTS /opt/homebrew/opt/flang/lib)
find_library(FORTRAN_SUPPORT_LIB FortranSupport HINTS /opt/homebrew/opt/flang/lib)
find_library(FORTRAN_DECIMAL_LIB FortranDecimal HINTS /opt/homebrew/opt/flang/lib)
find_library(LLVM_LIB LLVM-22 HINTS /opt/homebrew/opt/llvm/lib)

if(FLANG_INCLUDE_DIR AND LLVM_INCLUDE_DIR AND FORTRAN_PARSER_LIB AND FORTRAN_SEMANTICS_LIB AND FORTRAN_EVALUATE_LIB AND FORTRAN_SUPPORT_LIB AND FORTRAN_DECIMAL_LIB AND LLVM_LIB)
  add_executable(flang-inprocess-probe src/InProcessFlangProbe.cpp)
  target_include_directories(flang-inprocess-probe PRIVATE ${FLANG_INCLUDE_DIR} ${LLVM_INCLUDE_DIR})
  target_link_libraries(flang-inprocess-probe PRIVATE ${FORTRAN_PARSER_LIB} ${FORTRAN_SEMANTICS_LIB} ${FORTRAN_EVALUATE_LIB} ${FORTRAN_SUPPORT_LIB} ${FORTRAN_DECIMAL_LIB} ${LLVM_LIB})
  add_custom_target(flang-inprocess
    COMMAND flang-inprocess-probe examples/legacy/all_patterns.f
    DEPENDS flang-inprocess-probe
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Running in-process Flang parser and semantic analysis probe"
  )
endif()


add_test(NAME analyzer_tests
  COMMAND flang-modernizer-tests
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

find_program(FLANG_EXECUTABLE NAMES flang flang-new)

if(FLANG_EXECUTABLE)
  add_custom_target(flang-validate
    COMMAND ${FLANG_EXECUTABLE} -fsyntax-only examples/legacy/all_patterns.f
    COMMAND ${FLANG_EXECUTABLE} -fsyntax-only examples/case_study/solver.f examples/case_study/physics.f
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Validating legacy Fortran fixtures with LLVM Flang"
  )
else()
  add_custom_target(flang-validate
    COMMAND ${CMAKE_COMMAND} -E echo "LLVM Flang was not found on PATH. Install flang or add it to PATH."
  )
endif()

add_custom_target(flang-dump-tree
  COMMAND /bin/sh scripts/dump_flang_tree.sh ${FLANG_EXECUTABLE}
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  COMMENT "Writing Flang parse-tree and symbol dumps"
)

add_custom_target(report
  COMMAND flang-modernizer examples/case_study --output docs/case_study_report.md
  DEPENDS flang-modernizer
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  COMMENT "Generating case-study modernization report"
)

add_custom_target(transform
  COMMAND flang-modernizer examples/case_study --safe-transform-out transformed/case_study
  DEPENDS flang-modernizer
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  COMMENT "Generating safe transformed case-study files"
)

add_custom_target(serve
  COMMAND flang-modernizer --serve
  DEPENDS flang-modernizer
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  COMMENT "Starting local web dashboard at http://localhost:8080"
)


add_custom_target(real-report
  COMMAND flang-modernizer examples/real_case_study/minpack --output docs/real_case_study_report.md
  DEPENDS flang-modernizer
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  COMMENT "Generating real MINPACK case-study modernization report"
)

add_custom_target(real-transform
  COMMAND flang-modernizer examples/real_case_study/minpack --safe-transform-out transformed/real_case_study
  DEPENDS flang-modernizer
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  COMMENT "Generating safe transformed files for real MINPACK case study"
)

add_subdirectory(flang_ast_advisor)
