cmake_minimum_required(VERSION 3.22)

set(CMAKE_CXX_STANDARD 20)

add_executable(generator
        CreateTable.h
        CreateTable.cpp
        GetDirectionSymbols.h
        GetDirectionSymbols.cpp
        PrintTable.cpp
        PrintTable.h
        ReadGrammar.h
        ReadGrammar.cpp
        Rule.cpp
        Rule.h
        StringHelpers.h
        StringHelpers.cpp
        TableRow.h
        main.cpp
)
