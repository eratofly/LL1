#pragma once
#include <ostream>

#include "TableRow.h"

static const auto COLUMNS = "index\tsymbol\tdirectionSymbols\tshift\terror\tpointer\tstack\tend";
static const auto COLUMN_SEEPARATOR = "\t";
static const auto NULL_POINTER_VALUE = "NULL";

void PrintDirectionSymbols(const TableRow& tableStr, std::ostream& outputFile);

void PrintBoolValue(const bool value, std::ostream& outputFile);

void PrintPointer(const std::optional<size_t> pointer, std::ostream& outputFile);

void PrintTable(const Table& table, std::ostream& outputFile);