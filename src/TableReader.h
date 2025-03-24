#pragma once
#include <vector>
#include <string>
#include <fstream>
#include <set>
#include <sstream>
#include <optional>
#include "TableRow.h"
#include "StringHandler.h"

class TableReader {
public:
    static std::vector<TableRow> ReadFromFile(const std::string& fileName) {
        std::ifstream file(fileName);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open file: " + fileName);
        }

        SkipHeader(file);
        return ParseRows(file);
    }

private:
    static constexpr size_t EXPECTED_COLUMNS = 8;

    static void SkipHeader(std::ifstream& file) {
        std::string header;
        std::getline(file, header);
    }

    static std::vector<TableRow> ParseRows(std::ifstream& file) {
        std::vector<TableRow> table;
        std::string line;

        while (std::getline(file, line)) {
            if (!line.empty()) {
                table.push_back(ParseRow(line));
            }
        }

        return table;
    }

    static TableRow ParseRow(const std::string& line) {
        auto columns = StringHandler::Split(line, "\t");
        ValidateColumnCount(columns, EXPECTED_COLUMNS);

        return TableRow {
                .symbol = columns[1],
                .directionSymbols = ParseDirectionSymbols(columns[2]),
                .shift = ParseBoolean(columns[3]),
                .error = ParseBoolean(columns[4]),
                .pointer = ParsePointer(columns[5]),
                .stack = ParseBoolean(columns[6]),
                .end = ParseBoolean(columns[7])
        };
    }

    static std::set<std::string> ParseDirectionSymbols(const std::string& str) {
        if (str.empty()) return {};
        auto symbols = StringHandler::Split(str, " | ");
        return { symbols.begin(), symbols.end() };
    }

    static std::optional<size_t> ParsePointer(const std::string& str) {
        if (str == "NULL") return std::nullopt;

        size_t value;
        std::stringstream ss(str);
        if (!(ss >> value)) {
            throw std::logic_error("Invalid pointer format: " + str);
        }
        return value - 1;
    }

    static bool ParseBoolean(const std::string& str) {
        if (str == "+") return true;
        if (str == "-") return false;
        throw std::logic_error("Invalid boolean format: " + str);
    }

    static void ValidateColumnCount(const std::vector<std::string>& columns, size_t expected) {
        if (columns.size() != expected) {
            throw std::logic_error("Invalid number of columns: " + std::to_string(columns.size()));
        }
    }
};