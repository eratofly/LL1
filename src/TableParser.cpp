#include "TableParser.h"
#include <sstream>

void trim(string &str)
{
    str.erase(0, str.find_first_not_of(" \t"));
    str.erase(str.find_last_not_of(" \t") + 1);
}

void TableParser::ExtractNonterminals(const string &line, vector<string> &nonterminalsSet, set<string> &allSymbolsSet)
{
    regex nonterminalRegex(R"(<([^>]+)>)");
    auto it = sregex_iterator(line.begin(), line.end(), nonterminalRegex);

    for (; it != sregex_iterator(); ++it)
    {
        string nt = it->str(1);
        if (find(nonterminalsSet.begin(), nonterminalsSet.end(), nt) == nonterminalsSet.end())
        {
            nonterminalsSet.push_back(nt);
        }
        allSymbolsSet.insert(nt);
    }
}

void TableParser::ExtractDirectionSymbols(const string &line,
                                          map<string, vector<string>> &directionSymbolsWithNonterminalSet,
                                          set<string> &allSymbolsSet)
{
    // Найти позицию первого "-"
    size_t dashPos = line.find('-');
    if (dashPos == string::npos)
    {
        return; // Нет символа "-", пропускаем строку
    }

    // Определяем нетерминал до "-"
    string nonterminal = line.substr(0, dashPos);
    trim(nonterminal); // Убираем лишние пробелы
    if (nonterminal.front() == '<' && nonterminal.back() == '>')
    {
        nonterminal = nonterminal.substr(1, nonterminal.size() - 2); // Убираем <>
    }

    // Добавляем в множество символов
    allSymbolsSet.insert(nonterminal);

    // Найти часть после "/"
    size_t slashPos = line.find('/');
    if (slashPos == string::npos)
    {
        return; // Нет "/", значит, нет направляющих символов
    }

    string directionSymbolsStr = line.substr(slashPos + 1);
    vector<string> directionSymbols;
    stringstream ss(directionSymbolsStr);
    string symbol;

    while (getline(ss, symbol, ','))
    {
        trim(symbol);
        directionSymbols.push_back(symbol);
        allSymbolsSet.insert(symbol);
    }

    // Добавляем в map нетерминал и его направляющие символы
    directionSymbolsWithNonterminalSet[nonterminal] = directionSymbols;
}

void TableParser::ExtractAllSymbols(const string &line, set<string> &allSymbolsSet)
{
    stringstream wordsStream(line);
    string word;

    while (wordsStream >> word)
    {
        if (word.front() != '<' && word.back() != '>' && word.front() != '/' && word.front() != '-')
        {
            allSymbolsSet.insert(word);
        }
    }
}

// Основная функция обработки грамматики
void TableParser::ParseGrammar(const string &filename, vector<string> &nonterminalsSet,
                               map<string, vector<string>> &directionSymbolsWithNonterminalSet,
                               set<string> &allSymbolsSet)
{
    ifstream file(filename);
    if (!file)
    {
        cerr << "Ошибка открытия файла: " << filename << endl;
        return;
    }

    string line;

    while (getline(file, line))
    {
        vector<string> foundNonterminals;

        ExtractNonterminals(line, nonterminalsSet, allSymbolsSet);
        ExtractDirectionSymbols(line, directionSymbolsWithNonterminalSet, allSymbolsSet);
        ExtractAllSymbols(line, allSymbolsSet);
    }
}


void TableParser::InitializeTable(const vector<string> &nonterminalsSet,
                                  const map<string, vector<string>> &directionSymbolsWithNonterminalSet,
                                  Table &table)
{
    // Проверяем, что nonterminalsSet не пуст
    if (nonterminalsSet.empty()) {
        cerr << "Ошибка: nonterminalsSet пуст!" << endl;
        return;
    }

    size_t numRows = nonterminalsSet.size();
    size_t numCols = 8;

    // Инициализируем таблицу с заполнением по умолчанию
    table.resize(numRows, vector<string>(numCols, "-"));

    // Заполняем заголовок таблицы
    table[0][0] = "number";
    table[0][1] = "nonterminals";
    table[0][2] = "direction symbols";
    table[0][3] = "error";
    table[0][4] = "pointer";
    table[0][5] = "stack";
    table[0][6] = "shift";
    table[0][7] = "end";

    // Заполняем таблицу данными
    for (size_t i = 0; i < numRows; ++i) {
        const string &nonterminal = nonterminalsSet[i];

        // Проверяем, существует ли nonterminal в directionSymbolsWithNonterminalSet
        auto it = directionSymbolsWithNonterminalSet.find(nonterminal);
        if (it != directionSymbolsWithNonterminalSet.end()) {
            const vector<string> &symbols = it->second;

            // Убедимся, что не выходим за границы столбцов
            size_t numSymbols = min(symbols.size(), numCols);
            for (size_t j = 0; j < numSymbols; ++j) {
                table[i][j] = symbols[j];
            }
        }
    }
}