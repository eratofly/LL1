cmake_minimum_required(VERSION 3.22)

set(CMAKE_CXX_STANDARD 20)

add_executable(analyzator
        Analizator.h
        Analizator.cpp
        ReadTable.h
        ReadTable.cpp
        StringHelpers.h
        StringHelpers.cpp
        TableRow.h
        main.cpp
        ../lexer/reader/Reader.h
        ../lexer/rules/IdRule.h
        ../lexer/rules/NumberRule.h
        ../lexer/rules/ReservedWordsRule.h
        ../lexer/rules/SpecialCharsRule.h
        ../lexer/rules/StringRule.h
        ../lexer/Lexer.h
        ../lexer/Token.h
        LLParser.h
)