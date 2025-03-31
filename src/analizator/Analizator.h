#pragma once
#include <fstream>
#include <iostream>
#include <format>
#include <stdexcept>

#include "TableRow.h"
#include "StringHelpers.h"

// Runner for LL-1
void Analyze(const Table table, const std::string& inputFileName);