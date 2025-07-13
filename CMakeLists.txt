cmake_minimum_required(VERSION 3.28)
project(entropy)
set(CMAKE_CXX_STANDARD 23)
enable_testing()

add_executable(entropy entropy.cpp)
add_executable(sample sample.cpp)
add_executable(debug debug.cpp)

add_test(entropy entropy)
add_test(sample sample)
