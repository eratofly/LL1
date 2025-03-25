cmake_minimum_required(VERSION 3.22)
project(ll1)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(src)
add_executable(runner src/runner.cpp)
