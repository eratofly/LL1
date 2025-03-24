#pragma once
#include <vector>
#include <fstream>
#include <iostream>
#include <format>

#include "TableRow.h"
#include "StringHelpers.h"

class SyntaxAnalyzer {
public:
    [[maybe_unused]] void Analyze(const std::vector<TableRow>& transitionTable, const std::string& fileName) {
        ReadTokensFromFile(fileName);

        bool isAnalysisComplete = false;

        while (!isAnalysisComplete) {
            const TableRow& currentRow = transitionTable[currentState];
            std::optional<std::string> currentToken = GetCurrentToken();

            bool isTokenValid = currentToken.has_value() &&
                                currentRow.directionSymbols.find(currentToken.value()) != currentRow.directionSymbols.end();

            if (isTokenValid && stateStack.empty() && currentRow.end) {
                isAnalysisComplete = true;
                continue;
            }

            if (!isTokenValid && currentRow.error) {
                std::string errorMessage = currentToken.value_or("") + " is an incorrect symbol";
                throw std::runtime_error(errorMessage);
            }

            if (!isTokenValid) {
                currentState++;
                if (currentRow.shift) {
                    RemoveCurrentToken();
                }
                continue;
            }

            if (currentRow.stack) {
                stateStack.push_back(currentState + 1);
            }

            if (isTokenValid && currentRow.pointer.has_value()) {
                currentState = currentRow.pointer.value();
            }

            if (!currentRow.pointer.has_value()) {
                currentState = stateStack.back();
                stateStack.pop_back();
            }

            if (currentRow.shift) {
                RemoveCurrentToken();
            }
        }
    }

private:
    std::vector<std::string> tokenList;
    size_t currentState = 0;
    std::vector<size_t> stateStack;

    void ReadTokensFromFile(const std::string& fileName) {
        std::ifstream inputFile(fileName);

        if (!inputFile.is_open()) {
            throw std::runtime_error("Input file not found");
        }

        std::string line;
        while (std::getline(inputFile, line)) {
            size_t startPos = 0;
            size_t endPos = line.find(' ');
            while (endPos != std::string::npos) {
                std::string token = line.substr(startPos, endPos - startPos);
                if (!token.empty()) {
                    tokenList.push_back(token);
                }
                startPos = endPos + 1;
                endPos = line.find(' ', startPos);
            }
            if (startPos != line.length()) {
                std::string token = line.substr(startPos);
                tokenList.push_back(token);
            }
        }

        if (tokenList.empty()) {
            throw std::runtime_error("Input file is empty");
        }
    }

    [[nodiscard]] std::optional<std::string> GetCurrentToken() const {
        if (tokenList.empty()) {
            return std::nullopt;
        }
        return tokenList[0];
    }

    void RemoveCurrentToken() {
        if (!tokenList.empty()) {
            tokenList.erase(tokenList.begin());
        }
    }
};