#pragma once
#include <ostream>
#include <vector>

#include "TableRow.h"

void PrintDirectionSymbols(const TableRow& tableStr, std::ostream& outputFile);

void PrintBoolValue(bool value, std::ostream& outputFile);

void PrintPointer(std::optional<size_t> pointer, std::ostream& outputFile);

void PrintTable(const std::vector<TableRow>& table, std::ostream& outputFile);