cmake_minimum_required(VERSION 3.28)
project(entropy)
set(CMAKE_CXX_STANDARD 23)
enable_testing()

include_directories(src)

add_executable(tests tests/tests.cpp)
add_executable(sample tests/sample.cpp)
add_executable(bench tests/bench.cpp)

add_test(tests tests)
add_test(sample sample)
add_test(static bench static 1000000)
add_test(dynamic bench 6 1000000)
