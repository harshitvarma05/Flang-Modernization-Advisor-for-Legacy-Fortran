cmake_minimum_required(VERSION 3.16)
project(FlangModernizationAdvisor LANGUAGES CXX)

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

add_executable(flang-modernizer src/main.cpp)
target_link_libraries(flang-modernizer PRIVATE flang_modernizer_core)

add_executable(flang-modernizer-tests tests/test_analyzer.cpp)
target_link_libraries(flang-modernizer-tests PRIVATE flang_modernizer_core)

enable_testing()
add_test(NAME analyzer_tests COMMAND flang-modernizer-tests)
