#include "GrammarReader.h"
#include "GrammarRules.h"
#include "PrintTable.h"

#include <fstream>
#include <iostream>
#include <optional>
#include <vector>

#include "SyntaxAnalyzer.h"
#include "TableCreator.h"
#include "TableReader.h"
#include "TableRow.h"

struct Args
{
	std::string grammarFileName, outputFileName;
};

std::optional<Args> ParseArgs(int argc, char* argv[]);

int main(int argc, char* argv[])
{
	std::optional<Args> args = ParseArgs(argc, argv);
	if (!args.has_value())
	{
		return EXIT_FAILURE;
	}

	std::ifstream inputFile(args->grammarFileName);
	if (!inputFile.is_open())
	{
		std::cerr << "Input file is not found: " << args->grammarFileName << std::endl;
		return EXIT_FAILURE;
	}
	std::ofstream outputFile(args->outputFileName);
	if (!outputFile.is_open())
	{
		std::cerr << "Output file is not found: " << args->outputFileName << std::endl;
		return EXIT_FAILURE;
	}

	std::vector<GrammarRules::Rule> rules = GrammarReader::ReadGrammar(inputFile);
	for (const GrammarRules::Rule& rule : rules)
	{
		std::cout << rule.nonTerminal << " -> ";
		for (const std::string& s : rule.rightPart)
		{
			std::cout << s << " ";
		}
		if (!rule.directionSymbols.empty())
		{
			std::cout << "/ ";
		}
		for (const std::string s : rule.directionSymbols)
		{
			std::cout << s << " ";
		}
		std::cout << std::endl;
	}

	std::vector<TableRow> table = TableCreator::BuildTable(rules);
	PrintTable(table, outputFile);

	try
	{
		TableReader::ReadFromFile(args->outputFileName);
	}
	catch (const std::runtime_error&)
	{
		std::cerr << "Can not read table from file: " << args->outputFileName << std::endl;
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
	args.grammarFileName = argv[1];
	args.outputFileName = argv[2];

	return args;
}