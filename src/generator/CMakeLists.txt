cmake_minimum_required(VERSION 3.22)

set(CMAKE_CXX_STANDARD 20)

enable_testing()

add_library(catch2 INTERFACE)
target_include_directories(
        catch2 INTERFACE lib/catch2
)

add_executable(ll1_program
#        Analizator.h
#        Analizator.cpp
        CreateTable.h
        CreateTable.cpp
        GetDirectionSymbols.h
        GetDirectionSymbols.cpp
        PrintTable.cpp
        PrintTable.h
#        ReadTable.h
#        ReadTable.cpp
        ReadGrammar.h
        ReadGrammar.cpp
        Rule.cpp
        Rule.h
        StringHelpers.h
        StringHelpers.cpp
        TableRow.h
        main.cpp
)

#add_executable(ll1_tests
#)

#target_link_libraries(ll1_tests PUBLIC catch2)