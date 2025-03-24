cmake_minimum_required(VERSION 3.22)

set(CMAKE_CXX_STANDARD 20)

enable_testing()

add_library(catch2 INTERFACE)
target_include_directories(
        catch2 INTERFACE lib/catch2
)

add_executable(ll1_program
        TableCreator.h
        PrintTable.cpp
        PrintTable.h
        GrammarReader.h
        GrammarRules.h
        TableRow.h
        main.cpp
)

#add_executable(ll1_tests
#)

#target_link_libraries(ll1_tests PUBLIC catch2)