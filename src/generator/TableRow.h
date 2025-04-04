#pragma once
#include <optional>
#include <set>
#include <string>
#include <vector>

struct TableRow
{
	std::string symbol;
	std::set<std::string> directionSymbols;
	bool shift = false;
	bool error = false;
	std::optional<size_t> pointer;
	bool stack = false;
	bool end = false;
};

using Table = std::vector<TableRow>;