#include <algorithm>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>
#include <optional>
#include <fstream>
#include "Reader.h"

class SLRParser
{
public:
    void ComputeFirst(char symbol, std::set<char> &firstSet);

    void ComputeFollow(char symbol, std::set<char> &followSet, std::set<char> &processed);

    std::vector<std::pair<char, std::string>> Closure(const std::vector<std::pair<char, std::string>> &kernel);

    void BuildItemSets();

    void BuildParsingTable();

    SLRParser(const std::vector<Production> &prods, char start);

    void BuildParser();

    void DisplayResults() const;

    void WriteTableIntoCsv(std::ofstream &outputFile) const;

private:
    static constexpr auto EPSILON = '$';
    static constexpr auto END = '#';

    std::map<char, std::vector<std::string>> m_productions;
    std::vector<Production> m_productionsForReduces;
    std::map<char, std::set<char>> m_first, m_follow;
    std::vector<std::vector<std::pair<char, std::string>>> m_itemSet;
    std::map<std::pair<int, char>, int> m_gotoTable;
    std::map<std::pair<int, char>, std::vector<std::string>> m_actionTable;
    std::vector<char> m_terminals, m_nonTerminals;
    char m_startSymbol;

    int FindProductionIndex(const std::vector<Production> &productions,
                            char targetLeft,
                            const std::string &targetRight);
};

SLRParser::SLRParser(const std::vector<Production> &prods, char start)
        : m_startSymbol(start)
{
    m_productionsForReduces = prods;
    for (const auto &prod: prods)
    {
        m_productions[prod.left].push_back(prod.right);
        if (std::ranges::find(m_nonTerminals, prod.left) == m_nonTerminals.end())
        {
            m_nonTerminals.push_back(prod.left);
        }
        for (char c: prod.right)
        {
            if (std::isupper(c))
            {
                if (std::ranges::find(m_nonTerminals, c) == m_nonTerminals.end())
                {
                    m_nonTerminals.push_back(c);
                }
            } else if (std::ranges::find(m_terminals, c) == m_terminals.end())
            {
                m_terminals.push_back(c);
            }
        }
    }
    m_terminals.push_back(END);
}

void SLRParser::ComputeFirst(char symbol, std::set<char> &firstSet)
{
    if (m_first.contains(symbol))
    {
        firstSet.insert(m_first[symbol].begin(), m_first[symbol].end());
        return;
    }
    if (std::ranges::find(m_terminals, symbol) != m_terminals.end())
    {
        firstSet.insert(symbol);
        return;
    }
    for (const auto &rhs: m_productions[symbol])
    {
        if (rhs[0] == symbol)
            continue;
        bool allDeriveEpsilon = true;
        for (const char c: rhs)
        {
            std::set<char> tempFirst;
            ComputeFirst(c, tempFirst);
            firstSet.insert(tempFirst.begin(), tempFirst.end());
            if (!tempFirst.contains(EPSILON))
            {
                allDeriveEpsilon = false;
                break;
            }
        }
        if (allDeriveEpsilon)
        {
            firstSet.insert(EPSILON);
        }
    }
    m_first[symbol] = firstSet;
}

void SLRParser::ComputeFollow(char symbol, std::set<char> &followSet, std::set<char> &processed)
{
    if (processed.find(symbol) != processed.end())
        return;
    processed.insert(symbol);
    if (symbol == m_startSymbol)
    {
        followSet.insert(END);
    }
    for (const auto &[left, rights]: m_productions)
    {
        for (const auto &right: rights)
        {
            auto pos = right.find(symbol);
            if (pos != std::string::npos)
            {
                if (pos == right.length() - 1)
                {
                    if (left != symbol)
                    {
                        std::set<char> tempFollow;
                        ComputeFollow(left, tempFollow, processed);
                        followSet.insert(tempFollow.begin(), tempFollow.end());
                    }
                } else
                {
                    std::set<char> tempFirst;
                    ComputeFirst(right[pos + 1], tempFirst);
                    followSet.insert(tempFirst.begin(), tempFirst.end());
                    if (tempFirst.contains(EPSILON))
                    {
                        if (left != symbol)
                        {
                            std::set<char> tempFollow;
                            ComputeFollow(left, tempFollow, processed);
                            followSet.insert(tempFollow.begin(), tempFollow.end());
                        }
                    }
                }
            }
        }
    }
    m_follow[symbol] = followSet;
}

std::vector<std::pair<char, std::string>> SLRParser::Closure(const std::vector<std::pair<char, std::string>> &kernel)
{
    std::vector<std::pair<char, std::string>> result = kernel;
    bool changed;
    do
    {
        changed = false;
        std::vector<std::pair<char, std::string>> newItems;
        for (const auto &[left, right]: result)
        {
            auto dotPos = right.find('.');
            if (dotPos != right.length() - 1)
            {
                char nextSymbol = right[dotPos + 1];
                if (std::isupper(nextSymbol))
                {
                    for (const auto &prod: m_productions[nextSymbol])
                    {
                        std::pair newItem = {nextSymbol, "." + prod};
                        if (std::ranges::find(result, newItem) == result.end() &&
                            std::ranges::find(newItems, newItem) == newItems.end())
                        {
                            newItems.push_back(newItem);
                            changed = true;
                        }
                    }
                }
            }
        }
        result.insert(result.end(), newItems.begin(), newItems.end());
    } while (changed);
    return result;
}

void SLRParser::BuildItemSets()
{
    std::vector<std::pair<char, std::string>> initialItem = {{m_startSymbol, "." + m_productions[m_startSymbol][0]}};
    m_itemSet.push_back(Closure(initialItem));
    for (size_t i = 0; i < m_itemSet.size(); i++)
    {
        std::map<char, std::vector<std::pair<char, std::string>>> symbolGroups;
        for (const auto &[left, right]: m_itemSet[i])
        {
            auto dotPos = right.find('.');
            if (dotPos != right.length() - 1)
            {
                char nextSymbol = right[dotPos + 1];
                std::string newRight = right;
                std::swap(newRight[dotPos], newRight[dotPos + 1]);
                symbolGroups[nextSymbol].emplace_back(left, newRight);
            }
        }
        for (const auto &[symbol, group]: symbolGroups)
        {
            auto newItemSet = Closure(group);
            auto it = std::ranges::find(m_itemSet, newItemSet);
            if (it == m_itemSet.end())
            {
                m_gotoTable[{i, symbol}] = m_itemSet.size();
                m_itemSet.push_back(newItemSet);
            } else
            {
                m_gotoTable[{i, symbol}] = std::distance(m_itemSet.begin(), it);
            }
        }
    }
}

void SLRParser::BuildParsingTable()
{
    for (size_t i = 0; i < m_itemSet.size(); i++)
    {
        for (const auto &[left, right]: m_itemSet[i])
        {
            auto dotPos = right.find('.');
            if (dotPos == right.length() - 1)
            {
                if (left == m_startSymbol && right == m_productions[m_startSymbol][0] + ".")
                {
                    m_actionTable[{i, END}].emplace_back("accept");
                } else
                {
                    std::string productionRight = right.substr(0, right.length() - 1);
                    for (char terminal: m_follow[left])
                    {
                        std::stringstream ss;
                        int index = FindProductionIndex(m_productionsForReduces, left, productionRight);
                        ss << "R" << index + 1;
                        m_actionTable[{i, terminal}].push_back(ss.str());
                    }
                }
            } else
            {
                char nextSymbol = right[dotPos + 1];
                if (std::find(m_terminals.begin(), m_terminals.end(), nextSymbol) != m_terminals.end())
                {
                    std::stringstream ss;
                    ss << "shift " << m_gotoTable[{i, nextSymbol}];
                    m_actionTable[{i, nextSymbol}].push_back(ss.str());
                }
            }
        }
        for (char nonTerminal: m_nonTerminals)
        {
            auto it = m_gotoTable.find({i, nonTerminal});
            if (it != m_gotoTable.end())
            {
                std::stringstream ss;
                ss << it->second;
                m_actionTable[{i, nonTerminal}].push_back(ss.str());
            }
        }
    }
}

void SLRParser::BuildParser()
{
    for (char nonTerminal: m_nonTerminals)
    {
        std::set<char> firstSet;
        ComputeFirst(nonTerminal, firstSet);
    }
    for (char nonTerminal: m_nonTerminals)
    {
        std::set<char> followSet, processed;
        ComputeFollow(nonTerminal, followSet, processed);
    }
    BuildItemSets();
    BuildParsingTable();
}

void SLRParser::DisplayResults() const
{
    std::cout << "First sets:\n";
    for (const auto &[symbol, set]: m_first)
    {
        std::cout << symbol << ": {";
        for (auto it = set.begin(); it != set.end(); ++it)
        {
            if (it != set.begin())
                std::cout << ", ";
            std::cout << *it;
        }
        std::cout << "}\n";
    }
    std::cout << "\nFOLLOW sets:\n";
    for (const auto &[symbol, set]: m_follow)
    {
        std::cout << symbol << ": {";
        for (auto it = set.begin(); it != set.end(); ++it)
        {
            if (it != set.begin())
                std::cout << ", ";
            std::cout << *it;
        }
        std::cout << "}\n";
    }
    std::cout << "\nItem Sets:\n";
    for (size_t i = 0; i < m_itemSet.size(); i++)
    {
        std::cout << "I" << i << ":\n";
        for (const auto &[left, right]: m_itemSet[i])
        {
            std::cout << " " << left << " -> " << right << "\n";
        }
        std::cout << "\n";
    }
    std::cout << "Parsing Table:\n";
    std::cout << " ";
    for (char terminal: m_terminals)
        // std::cout << terminal << "\t";
        std::cout << terminal << ",";
    for (char nonTerminal: m_nonTerminals)
        std::cout << nonTerminal << ",";
    // std::cout << nonTerminal << "\t";
    std::cout << "\n";
    for (size_t i = 0; i < m_itemSet.size(); i++)
    {
        std::cout << i << " ";
        for (char symbol: m_terminals)
        {
            auto it = m_actionTable.find({i, symbol});
            if (it != m_actionTable.end())
            {
                for (const auto &action: it->second)
                {
                    std::cout << action << " ";
                }
            }
            // std::cout << "\t";
            std::cout << ",";
        }
        for (char symbol: m_nonTerminals)
        {
            auto it = m_actionTable.find({i, symbol});
            if (it != m_actionTable.end())
            {
                for (const auto &action: it->second)
                {
                    std::cout << action << " ";
                }
            }
            // std::cout << "\t";
            std::cout << ",";
        }
        std::cout << "\n";
    }
}

void SLRParser::WriteTableIntoCsv(std::ofstream &outputFile) const
{
    outputFile << "State ";
    for (char terminal: m_terminals)
        outputFile << terminal << "\t";
    for (char nonTerminal: m_nonTerminals)
        outputFile << nonTerminal << "\t";
    outputFile << "\n";

    for (size_t i = 0; i < m_itemSet.size(); i++)
    {
        outputFile << i << "\t";
        for (char symbol: m_terminals)
        {
            auto it = m_actionTable.find({i, symbol});
            if (it != m_actionTable.end())
            {
                for (const auto &action: it->second)
                {
                    outputFile << action << "\t";
                }
            }
            outputFile << "\t";
        }
        for (char symbol: m_nonTerminals)
        {
            auto it = m_actionTable.find({i, symbol});
            if (it != m_actionTable.end())
            {
                for (const auto &action: it->second)
                {
                    outputFile << action << "\t";
                }
            }
            outputFile << "\t";
        }
        outputFile << "\n";
    }

    outputFile.close();
}

int SLRParser::FindProductionIndex(const std::vector<Production> &productions,
                                   char targetLeft,
                                   const std::string &targetRight)
{
    auto it = std::find_if(productions.begin(), productions.end(),
                           [&](const Production &p)
                           {
                               return p.left == targetLeft && p.right == targetRight;
                           });

    if (it != productions.end())
    {
        return std::distance(productions.begin(), it);
    }
    return -1;
}

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
        return 1;
    }

    std::ifstream inputFile(args->inputFileName);
    if (!inputFile.is_open())
    {
        std::cout << "Input file is not found: " << args->inputFileName << std::endl;
        return 1;
    }

    std::ofstream outputFile(args->outputFileName);
    if (!outputFile.is_open())
    {
        std::cout << "Output file is not found: " << args->outputFileName << std::endl;
        return 1;
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