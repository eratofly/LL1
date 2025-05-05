cmake_minimum_required(VERSION 3.22)

set(CMAKE_CXX_STANDARD 20)

add_executable(slr
        main.cpp
        Reader.h
        Production.h
        SLRParser.h
)
