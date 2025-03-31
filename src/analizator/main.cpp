#include <iostream>
#include <optional>
#include <vector>
#include <fstream>

#include "TableRow.h"
#include "ReadTable.h"
#include "Analizator.h"

struct Args
{
    std::string inputFileName, grammarFileName;
};

std::optional<Args> ParseArgs(int argc, char* argv[]);

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
        Analyze(table, args->inputFileName);
        std::cout << "OK!!!" << std::endl;
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