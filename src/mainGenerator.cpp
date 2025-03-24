#include <iostream>
#include <optional>
#include <fstream>
#include <iostream>

#include "CreateTable.h"
#include "PrintTable.h"
#include "ReadGrammar.h"
#include "SyntaxAnalyzer.h"

struct Args
{
    std::string inputFileName, outputFileName;
};

std::optional<Args> ParseArgs(int argc, char* argv[]);

int main(int argc, char* argv[])
{
    std::optional<Args> args = ParseArgs(argc, argv);
    if (args == std::nullopt)
    {
        return EXIT_FAILURE;
    }

    std::ifstream inputFile(args->inputFileName);
    if (!inputFile.is_open())
    {
        std::cerr << "Input file is not found: " << args->inputFileName << std::endl;
        return EXIT_FAILURE;
    }

    std::ofstream outputFile(args->outputFileName);
    if (!outputFile.is_open())
    {
        std::cerr << "Output file is not found: " << args->outputFileName << std::endl;
        return EXIT_FAILURE;
    }

    std::vector<Rule> rules = ReadGrammar(inputFile);

    for (const Rule& rule : rules)
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

    std::vector<TableRow> table = CreateTable(rules);
    PrintTable(table, outputFile);

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
    args.outputFileName = argv[2];

    return args;
}