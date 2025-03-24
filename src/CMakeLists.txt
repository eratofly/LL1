cmake_minimum_required(VERSION 3.22)

set(CMAKE_CXX_STANDARD 20)

enable_testing()

add_library(catch2 INTERFACE)
target_include_directories(
        catch2 INTERFACE lib/catch2
)

add_executable(ll1_program_1
        CreateTable.h
        PrintTable.cpp
        PrintTable.h
        ReadGrammar.cpp
        ReadGrammar.h
        GrammarRules.h
        StringHelpers.cpp
        StringHelpers.h
        TableRow.h
        main.cpp
)

add_executable(ll1_program_2
        mainGenerator.cpp
        CreateTable.h
        PrintTable.cpp
        PrintTable.h
        ReadGrammar.cpp
        ReadGrammar.h
        Rule.cpp
        Rule.h
        StringHelpers.cpp
        StringHelpers.h
        TableRow.h
        SyntaxAnalyzer.h
)