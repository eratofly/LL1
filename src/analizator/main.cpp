#include <fstream>
#include <iostream>
#include <optional>
#include <vector>

#include "Analizator.h"
#include "LLParser.h"
#include "ReadTable.h"
#include "TableRow.h"

struct Args
{
	std::string inputFileName, grammarFileName;
};

std::optional<Args> ParseArgs(int argc, char* argv[]);

std::string GetInputGrammarFromFile(std::string const& inputFileName)
{
	std::ifstream input(inputFileName);
	std::string line;
	std::getline(input, line);

	return line;
}

int main(int argc, char* argv[])
{
	auto args = ParseArgs(argc, argv);

	if (!args.has_value())
	{
		return EXIT_FAILURE;
	}

	std::vector<TableRow> table;

	try
	{
		table = ReadTable(args->grammarFileName);
	}
	catch (const std::runtime_error&)
	{
		return EXIT_FAILURE;
	}

	try
	{
		// Analyze(table, args->inputFileName);
		LLParser parser(table);
		if (parser.Parse(GetInputGrammarFromFile(args->inputFileName)))
		{
			std::cout << "OK!!!" << std::endl;
		}
		else
		{
			auto error = parser.GetError();
			std::cout << "ERROR!!!" << std::endl;
			std::cout << "Get token: " << error.received.value << " in position " << error.received.pos << std::endl
					  << "Expected symbols: ";

			for (const auto& symbol : error.expected)
			{
				std::cout << symbol;
				if (symbol != *error.expected.rbegin())
				{
					std::cout << ", ";
				}
			}
			std::cout << std::endl;
		}
	}
	catch (const std::exception& e)
	{
		std::cout << "ERROR: " << e.what() << std::endl;
	}

	return EXIT_SUCCESS;
}

std::optional<Args> ParseArgs(int argc, char* argv[])
{
	if (argc != 3)
	{
		std::cout << "Invalid quantity of arguments" << std::endl;
		return std::nullopt;
	}

	Args args;
	args.inputFileName = argv[1];
	args.grammarFileName = argv[2];

	return args;
}