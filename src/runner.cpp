#include "GrammarReader.h"
#include "PrintTable.h"

#include <fstream>
#include <iostream>
#include <optional>
#include <vector>

#include "SyntaxAnalyzer.h"
#include "TableReader.h"

struct Args
{
	std::string tableFileName, inputStringFile;
};

std::optional<Args> ParseArgs(int argc, char* argv[]);

int main(int argc, char* argv[])
{
	std::optional<Args> args = ParseArgs(argc, argv);
	if (!args.has_value())
	{
		return EXIT_FAILURE;
	}

	auto tableRows = TableReader::ReadFromFile(args->tableFileName);
	std::ifstream ifs(args->inputStringFile);
	if (!ifs.is_open())
	{
		throw std::invalid_argument("Could not open file " + args->inputStringFile);
	}
	std::string inputStr(std::istreambuf_iterator{ifs}, {});

	try
	{
		SyntaxAnalyzer syntaxAnalyzer;
		if (syntaxAnalyzer.Run(tableRows, inputStr))
		{
			std::cout << "Runner complete string: " << inputStr << std::endl;
		}
		else
		{
			std::cerr << "Failed to run string: " << inputStr << std::endl;
		}

		std::cout << syntaxAnalyzer.GetPathway() << std::endl;
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << std::endl;
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}

std::optional<Args> ParseArgs(int argc, char* argv[])
{
	if (argc != 3)
	{
		std::cerr << "Invalid quantity of arguments" << std::endl;
		return std::nullopt;
	}

	Args args;
	args.tableFileName = argv[1];
	args.inputStringFile = argv[2];

	return args;
}