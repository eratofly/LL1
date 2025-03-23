#ifndef TABLEPARSER_H
#define TABLEPARSER_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <regex>

using namespace std;

using Table = vector<vector<string>>;

class TableParser {
public:
    static void ParseGrammar(const string& filename, vector<string>& nonterminalsSet,
                             map<string, vector<string>>& directionSymbolsWithNonterminalSet,
                             set<string>& allSymbolsSet);

    static void InitializeTable(const vector<string> &nonterminalsSet,
                         const map<string, vector<string>> &directionSymbolsWithNonterminalSet, Table &table);

private:
    static void ExtractNonterminals(const string& line, vector<string>& nonterminalsSet, set<string>& allSymbolsSet);
    static void ExtractDirectionSymbols(const string &line,
                                              map<string, vector<string>> &directionSymbolsWithNonterminalSet,
                                              set<string> &allSymbolsSet);
    static void ExtractAllSymbols(const string& line, set<string>& allSymbolsSet);

};

#endif // TABLEPARSER_H
