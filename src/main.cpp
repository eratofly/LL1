#include <fstream>
#include <iostream>
#include <optional>
#include <vector>

#include "ReadTable.h"
#include "SyntaxAnalyzer.h"
#include "TableRow.h"

struct Args
{
	std::string inputFileName, grammarFileName;
};

std::optional<Args> ParseArgs(int argc, char* argv[]);

int main(int argc, char* argv[])
{
	std::optional<Args> args = ParseArgs(argc, argv);
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
		std::cerr << "Can not read table from file: " << args->grammarFileName << std::endl;
		return EXIT_FAILURE;
	}

	try
	{
		SyntaxAnalyzer().Analyze(table, args->inputFileName);
		std::cout << "Success" << std::endl;
	}
	catch (const std::exception& e)
	{
		std::cout << "Error: " << e.what() << std::endl;
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
	args.inputFileName = argv[1];
	args.grammarFileName = argv[2];

	return args;
}