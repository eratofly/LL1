#include "Reader.h"
#include "SLRParser.h"

#include <optional>

struct Args
{
    std::string inputFileName, outputFileName;
};

std::optional<Args> ParseArgs(int argc, char *argv[])
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

int main(int argc, char *argv[])
{
    std::optional<Args> args = ParseArgs(argc, argv);
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


//    std::vector<SLRParser::Production> productions = { { 'S', "E" },
//		{ 'E', "E+T" }, { 'E', "T" }, { 'T', "T*F" },
//		{ 'T', "F" }, { 'F', "(E)" }, { 'F', "id" } };
    std::vector<Production> productions = Reader::ReadGrammarRules(inputFile);
    auto startWith = productions[0].left;
    SLRParser parser(productions, startWith);
    parser.BuildParser();
    parser.DisplayResults();
    parser.WriteTableIntoCsv(outputFile);

    return EXIT_SUCCESS;
}