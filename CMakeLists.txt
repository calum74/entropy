cmake_minimum_required(VERSION 3.28)
project(entropy)
set(CMAKE_CXX_STANDARD 23)

add_executable(entropy entropy.cpp)
add_executable(sample sample.cpp)
