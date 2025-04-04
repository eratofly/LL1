#include "PrintTable.h"

#include <cstring>

static constexpr char NON_TERMINAL_CHARS_TO_REMOVE[] = "<>";

void PrintDirectionSymbols(const TableRow& tableStr, std::ostream& outputFile)
{
	int index = 0;
	for (const std::string& directionSymbol : tableStr.directionSymbols)
	{
		std::string symbol = directionSymbol == ";" ? ".," : directionSymbol;
		outputFile << directionSymbol;
		if (index != tableStr.directionSymbols.size() - 1)
		{
			outputFile << " | ";
		}
		index++;
	}
	outputFile << COLUMN_SEEPARATOR;
}

void PrintBoolValue(const bool value, std::ostream& outputFile)
{
	if (value)
	{
		outputFile << "+";
	}
	else
	{
		outputFile << "-";
	}
	outputFile << COLUMN_SEEPARATOR;
}

void PrintPointer(const std::optional<size_t> pointer, std::ostream& outputFile)
{
	if (pointer.has_value())
	{
		outputFile << pointer.value() + 1;
	}
	else
	{
		outputFile << NULL_POINTER_VALUE;
	}
	outputFile << COLUMN_SEEPARATOR;
}

std::string GetChangedSymbol(std::string& symbol)
{
	if (symbol.size() > 1)
	{
		for (unsigned int i = 0; i < strlen(NON_TERMINAL_CHARS_TO_REMOVE); ++i)
		{
			auto pos = symbol.find_first_of(NON_TERMINAL_CHARS_TO_REMOVE[i]);
			if (pos != std::string::npos)
			{
				symbol.erase(pos, 1);
			}
		}
	}
	return symbol;
}

void PrintTable(const Table& table, std::ostream& outputFile)
{
	outputFile << COLUMNS << std::endl;

	for (size_t i = 0; i < table.size(); i++)
	{
		const TableRow& tableStr = table[i];
		std::string symbol = tableStr.symbol == ";" ? "\";\"" : tableStr.symbol;
		outputFile << i + 1 << COLUMN_SEEPARATOR
				   << GetChangedSymbol(symbol) << COLUMN_SEEPARATOR;
		PrintDirectionSymbols(tableStr, outputFile);
		PrintBoolValue(tableStr.shift, outputFile);
		PrintBoolValue(tableStr.error, outputFile);
		PrintPointer(tableStr.pointer, outputFile);
		PrintBoolValue(tableStr.stack, outputFile);
		PrintBoolValue(tableStr.end, outputFile);
		outputFile << std::endl;
	}
}