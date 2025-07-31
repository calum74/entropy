cmake_minimum_required(VERSION 3.28)
project(entropy)
set(CMAKE_CXX_STANDARD 23)
enable_testing()

include_directories(src)
include_directories(third-party/fast-loaded-dice-roller/src/c)
include_directories(third-party/amplified-loaded-dice-roller)

add_library(fldr third-party/fast-loaded-dice-roller/src/c/fldr.c)
add_library(aldr third-party/amplified-loaded-dice-roller/aldr.c)
add_library(testing src/entropy_store.c tests/fetch.cpp)
link_libraries(fldr)
link_libraries(testing)

add_executable(tests tests/tests.cpp)
add_executable(sample tests/sample.cpp)
add_executable(bench tests/bench.cpp)


add_test(tests tests)
add_test(sample sample)
add_test(bench bench)
