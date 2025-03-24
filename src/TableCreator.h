#ifndef TABLE_CREATOR_H
#define TABLE_CREATOR_H

#include <vector>
#include <string>
#include <set>
#include <optional>
#include "GrammarRules.h"
#include "TableRow.h"
#include "DirectionSymbolsProcessor.h"

class TableCreator {
public:
    static std::vector<TableRow> BuildTable(const std::vector<GrammarRules::Rule>& rules) {
        std::vector<TableRow> table;
        AddNonTerminalRows(rules, table);

        for (size_t i = 0; i < rules.size(); ++i) {
            table[i].pointer = table.size();
            table[i].error = !HasNonTerminalAfterIndex(i + 1, rules, rules[i].nonTerminal);
            AddRightPartRows(rules, table, rules[i]);
        }

        return table;
    }

private:
    static bool HasNonTerminalAfterIndex(size_t startIndex, const std::vector<GrammarRules::Rule>& rules, const std::string& nonTerminal) {
        for (size_t i = startIndex; i < rules.size(); ++i) {
            if (rules[i].nonTerminal == nonTerminal) {
                return true;
            }
        }
        return false;
    }

    static void AddNonTerminalRows(const std::vector<GrammarRules::Rule>& rules, std::vector<TableRow>& table) {
        for (const GrammarRules::Rule& rule : rules) {
            TableRow row;
            row.symbol = rule.nonTerminal;
            row.directionSymbols = rule.directionSymbols;
            table.push_back(row);
        }
    }

    static std::set<std::string> GetTerminalGuideSymbols(const std::vector<GrammarRules::Rule>& rules, const std::string& terminal, const std::string& nonTerminal) {
        if (terminal == GrammarRules::EMPTY_SYMBOL) {
            return DirectionSymbolsProcessor::GetDirectionSymbolsAfterNonTerminalInGrammar(nonTerminal, rules);
        }
        return { terminal };
    }

    static void AddRightPartRows(const std::vector<GrammarRules::Rule>& rules, std::vector<TableRow>& table, const GrammarRules::Rule& rule) {
        for (size_t i = 0; i < rule.rightPart.size(); ++i) {
            const std::string& symbol = rule.rightPart[i];
            TableRow row;
            row.symbol = symbol;
            bool isNonTerminal = GrammarRules::IsNonTerminalSymbol(symbol, rules);
            row.error = true;
            row.end = false;

            if (isNonTerminal) {
                row.directionSymbols = DirectionSymbolsProcessor::GetNonTerminalDirectionSymbols(symbol, rules);
                row.shift = false;
                row.stack = ShouldStack(rule, i);
                row.pointer = GrammarRules::FindNonTerminalIndex(rules, symbol);
            } else {
                std::set<std::string> guideSymbols = GetTerminalGuideSymbols(rules, symbol, rule.nonTerminal);
                row.directionSymbols.insert(guideSymbols.begin(), guideSymbols.end());
                row.shift = symbol != GrammarRules::END_SYMBOL && symbol != GrammarRules::EMPTY_SYMBOL;
                row.end = symbol == GrammarRules::END_SYMBOL;
                bool isLastSymbol = i == rule.rightPart.size() - 1;
                if (!isLastSymbol) {
                    row.pointer = table.size() + 1;
                }
            }
            table.push_back(row);
        }

        if (rule.hasEnd && rule.rightPart.back() != GrammarRules::END_SYMBOL) {
            TableRow row;
            row.directionSymbols.insert(GrammarRules::END_SYMBOL);
            row.symbol = GrammarRules::END_SYMBOL;
            row.error = true;
            row.shift = true;
            row.end = true;
            table.push_back(row);
        }
    }

    static bool ShouldStack(const GrammarRules::Rule& rule, size_t index) {
        size_t lastIndex = rule.rightPart.size() - 1;
        if (lastIndex != index) {
            return true;
        }
        return rule.hasEnd;
    }
};

#endif // TABLE_CREATOR_H