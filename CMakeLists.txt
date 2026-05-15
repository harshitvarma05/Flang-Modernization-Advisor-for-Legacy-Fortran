cmake_minimum_required(VERSION 3.16)
project(FlangModernizationAdvisor LANGUAGES CXX)

include(CTest)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(flang_modernizer_core
  src/Analyzer.cpp
  src/Models.cpp
  src/Reporter.cpp
  src/Transform.cpp
)
target_include_directories(flang_modernizer_core PUBLIC include)

add_executable(flang-modernizer src/main.cpp src/WebServer.cpp)
target_link_libraries(flang-modernizer PRIVATE flang_modernizer_core)

add_executable(flang-modernizer-tests tests/test_analyzer.cpp)
target_link_libraries(flang-modernizer-tests PRIVATE flang_modernizer_core)

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
