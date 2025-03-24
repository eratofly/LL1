#pragma once
#include <string>
#include <set>
#include <optional>

struct TableRow
{
	std::string symbol;
	std::set<std::string> directionSymbols;
	bool shift;
	bool error;
	std::optional<size_t> pointer;
	bool stack;
	bool end;
};