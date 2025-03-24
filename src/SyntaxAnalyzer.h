#pragma once
#include <vector>
#include <fstream>
#include <iostream>
#include <optional>
#include <string>
#include <set>
#include <algorithm>
#include "TableRow.h"
#include "CreateTable.h"

class SyntaxAnalyzer {
private:
    std::vector<std::string> m_tokenList;
    size_t m_currentState = 0;
    std::vector<size_t> m_stateStack;

    void ReadTokensFromFile(const std::string &fileName) {
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
                    m_tokenList.push_back(token);
                }
                startPos = endPos + 1;
                endPos = line.find(' ', startPos);
            }
            if (startPos != line.length()) {
                std::string token = line.substr(startPos);
                m_tokenList.push_back(token);
            }
        }

        if (m_tokenList.empty()) {
            throw std::runtime_error("Input file is empty");
        }
    }

    std::optional<std::string> GetCurrentToken() const {
        if (m_tokenList.empty()) {
            return std::nullopt;
        }
        return m_tokenList[0];
    }

    void RemoveCurrentToken() {
        if (!m_tokenList.empty()) {
            m_tokenList.erase(m_tokenList.begin());
        }
    }

public:
    void Analyze(const std::vector<TableRow> &transitionTable, const std::string &fileName) {
        ReadTokensFromFile(fileName);

        bool isAnalysisComplete = false;

        while (!isAnalysisComplete) {
            const TableRow &currentRow = transitionTable[m_currentState];
            std::optional<std::string> currentToken = GetCurrentToken();

            bool isTokenValid = currentToken.has_value() &&
                                currentRow.directionSymbols.find(currentToken.value()) !=
                                currentRow.directionSymbols.end();

            if (isTokenValid && m_stateStack.empty() && currentRow.end) {
                isAnalysisComplete = true;
                continue;
            }

            if (!isTokenValid && currentRow.error) {
                std::string errorMessage = currentToken.value_or("") + " is an incorrect symbol";
                throw std::runtime_error(errorMessage);
            }

            if (!isTokenValid) {
                m_currentState++;
                if (currentRow.shift) {
                    RemoveCurrentToken();
                }
                continue;
            }

            if (currentRow.stack) {
                m_stateStack.push_back(m_currentState + 1);
            }

            if (isTokenValid && currentRow.pointer.has_value()) {
                m_currentState = currentRow.pointer.value();
            }

            if (!currentRow.pointer.has_value()) {
                m_currentState = m_stateStack.back();
                m_stateStack.pop_back();
            }

            if (currentRow.shift) {
                RemoveCurrentToken();
            }
        }
    }
};