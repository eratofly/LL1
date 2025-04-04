#include <fstream>

#include "ReadGrammar.h"
#include "CreateTable.h"
#include "PrintTable.h"

struct Args
{
    std::string inputFileName, outputFileName;
};

std::optional<Args> ParseArgs(int argc, char* argv[]);

int main(int argc, char* argv[])
{
    auto args = ParseArgs(argc, argv);
    if (args == std::nullopt)
    {
        return EXIT_FAILURE;
    }

    std::ifstream inputFile(args->inputFileName);
    if (!inputFile.is_open())
    {
        std::cout << "Input file is not found: " << args->inputFileName << std::endl;
        return EXIT_FAILURE;
    }

    std::ofstream outputFile(args->outputFileName);
    if (!outputFile.is_open())
    {
        std::cout << "Output file is not found: " << args->outputFileName << std::endl;
        return EXIT_FAILURE;
    }

    auto rules = ReadGrammar(inputFile);

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
        for (const std::string& s : rule.directionSymbols)
        {
            std::cout << s << " ";
        }
        std::cout << std::endl;
    }

    auto table = CreateTable(rules);
    PrintTable(table, outputFile);
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
    args.outputFileName = argv[2];

    return args;
}