#pragma once
#include <vector>
#include <string>
#include <fstream>
#include <set>
#include <sstream>

#include "TableRow.h"
#include "StringHelpers.h"

static const auto NULL_POINTER_STRING = "NULL";
static const auto DIRECTED_SYMBOLS_SEPARATOR = " | ";

std::vector<TableRow> ReadTable(const std::string& grammarFileName);
TableRow ReadTableRow(const std::string& tableRowStr);
std::set<std::string> ReadDirectionSymbols(const std::string& directionSymbolsStr);
std::optional<size_t> ReadPointer(const std::string& pointerStr);