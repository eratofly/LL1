#pragma once
#include <fstream>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#include "StringHelpers.h"
#include "TableRow.h"

static inline const auto SPLIT_SYMBOL = "\t";
static inline const auto TABLE_TRUE_SYMBOL = "+";
static inline const auto TABLE_NULL = "NULL";

inline std::set<std::string> ReadDirectionSymbols(const std::string& directionSymbolsStr)
{
	std::vector<std::string> directionSymbols = Split(directionSymbolsStr, " | ");
	return { directionSymbols.begin(), directionSymbols.end() };
}

inline std::optional<size_t> ReadPointer(const std::string& pointerStr)
{
	if (pointerStr == TABLE_NULL)
	{
		return std::nullopt;
	}

	std::stringstream stream(pointerStr);
	size_t pointer;
	stream >> pointer;

	return --pointer;
}

inline TableRow ReadTableRow(const std::string& tableRowStr)
{
	const auto splitStr = Split(tableRowStr, SPLIT_SYMBOL);
	if (splitStr.size() != 8)
	{
		throw std::logic_error("Uncorrected quantity of parts of row");
	}

	return TableRow{
		.symbol = splitStr[1],
		.directionSymbols = ReadDirectionSymbols(splitStr[2]),
		.shift = splitStr[3] == TABLE_TRUE_SYMBOL,
		.error = splitStr[4] == TABLE_TRUE_SYMBOL,
		.pointer = ReadPointer(splitStr[5]),
		.stack = splitStr[6] == TABLE_TRUE_SYMBOL,
		.end = splitStr[7] == TABLE_TRUE_SYMBOL,
	};
}

inline std::vector<TableRow> ReadTable(const std::string& grammarFileName)
{
	std::ifstream grammarFile(grammarFileName);
	if (!grammarFile.is_open())
	{
		throw std::runtime_error("Grammar file is not found");
	}

	std::vector<TableRow> table;
	std::string str;

	getline(grammarFile, str); // skip first line
	while (getline(grammarFile, str))
	{
		table.push_back(ReadTableRow(str));
	}

	return table;
}