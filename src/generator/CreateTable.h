#pragma once
#include "Rule.h"
#include "TableRow.h"
#include "GetDirectionSymbols.h"

bool HasNonTerminal(size_t beginIndex, const std::vector<Rule>& rules, const std::string& nonTerminalName);

int GetCountOfIdenticalNonterminals(const std::vector<Rule>& rules, const std::string& nonTerminal);

void AddNonterminals(const std::vector<Rule>& rules, Table& table);

std::set<std::string> GetTerminalDirectionSymbols(const std::vector<Rule>& rules, const std::string& terminalName, const std::string& nonTerminalName);

void AddOtherTableRowFromRightPart(const std::vector<Rule>& rules, Table& table, const Rule& rule);

bool IsStack(const Rule& rule, const size_t index);

Table CreateTable(const std::vector<Rule>& rules);

bool IsEndOfRule(const Rule& rule, const size_t index);