cmake_minimum_required(VERSION 3.26)
project(ll1)

set(CMAKE_CXX_STANDARD 20)

#enable_testing()
#
#add_library(catch2 INTERFACE)
#target_include_directories(
#        catch2 INTERFACE lib/catch2
#)

add_executable(ll1_program
        src/main.cpp
        src/LLTableReader.h
)

#add_executable(ll1_tests
#)
#
#target_link_libraries(parser_tests PUBLIC catch2)