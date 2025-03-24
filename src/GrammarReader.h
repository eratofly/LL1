#ifndef GRAMMAR_PROCESSOR_H
#define GRAMMAR_PROCESSOR_H

#include <vector>
#include <string>
#include <set>
#include <map>
#include <algorithm>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include "GrammarRules.h"
#include "DirectionSymbolsProcessor.h"
#include "StringHelpers.h"

class GrammarReader {
public:
    static std::vector<GrammarRules::Rule> ReadGrammar(std::istream& inputFile) {
        std::vector<GrammarRules::Rule> rules;
        std::string ruleStr;
        while (getline(inputFile, ruleStr)) {
            std::vector<std::string> ruleParts = Split(ruleStr, "->");
            std::string nonTerminal = RemoveSpacesInBeginAndEndOfWord(ruleParts[0]);
            ReadRightPart(ruleParts[1], nonTerminal, rules);
        }
        RemoveLeftRecursion(rules);
        UpdateRulesWithSamePrefix(rules);
        DirectionSymbolsProcessor::ProcessDirectionSymbols(rules);
        return rules;
    }

private:
    static std::string GetUniqueNonterminalName(const std::vector<GrammarRules::Rule>& rules, const std::string& baseName) {
        int index = 1;
        std::string newName = baseName + std::to_string(index);
        while (!GrammarRules::GetRulesByNonTerminal(rules, newName).empty()) {
            index++;
            newName = baseName + std::to_string(index);
        }
        return newName;
    }

    static void ReadRightPart(const std::string& rightPartStr, const std::string& nonTerminal, std::vector<GrammarRules::Rule>& rules) {
        std::vector<std::string> rightParts = Split(rightPartStr, "|");
        for (std::string& part : rightParts) {
            part = RemoveSpacesInBeginAndEndOfWord(part);
        }
        for (const std::string& part : rightParts) {
            GrammarRules::Rule rule;
            rule.nonTerminal = nonTerminal;
            rule.rightPart = Split(part, " ");
            if (rules.empty()) {
                rule.hasEnd = true;
            }
            rules.push_back(rule);
        }
    }

    static void RemoveLeftRecursion(std::vector<GrammarRules::Rule>& rules) {
        std::vector<GrammarRules::Rule> newRules;
        bool hasChanges = false;

        for (size_t i = 0; i < rules.size(); ++i) {
            GrammarRules::Rule& rule = rules[i];
            if (rule.rightPart.empty()) {
                throw std::logic_error("Rule has no right part");
            }
            if (rule.rightPart.size() == 1 && rule.rightPart[0] == rule.nonTerminal) {
                throw std::logic_error("ERROR: <Nonterminal> -> <Nonterminal>");
            }
            if (rule.nonTerminal != rule.rightPart[0]) {
                newRules.push_back(rule);
                continue;
            }
            hasChanges = true;
            UpdateRuleWithLeftRecursion(rule, rules, newRules);
        }

        rules = newRules;

        if (hasChanges) {
            RemoveLeftRecursion(rules);
        }
    }

    static void UpdateRuleWithLeftRecursion(GrammarRules::Rule& rule, std::vector<GrammarRules::Rule>& rules, std::vector<GrammarRules::Rule>& newRules) {
        std::vector<GrammarRules::Rule> rulesWithNonterminal = GrammarRules::GetRulesByNonTerminal(rules, rule.nonTerminal);
        if (rulesWithNonterminal.size() < 2) {
            throw std::logic_error("Left recursion rule is looped");
        }

        std::string newNonterminalName = GetUniqueNonterminalName(rules, rule.nonTerminal);

        GrammarRules::Rule newRule;
        newRule.nonTerminal = newNonterminalName;
        newRule.hasEnd = rule.hasEnd;
        for (size_t i = 1; i < rule.rightPart.size(); ++i) {
            newRule.rightPart.push_back(rule.rightPart[i]);
        }
        newRule.rightPart.push_back(newNonterminalName);

        GrammarRules::Rule newEmptyRule;
        newEmptyRule.nonTerminal = newNonterminalName;
        newEmptyRule.rightPart.push_back(GrammarRules::EMPTY_SYMBOL);

        for (const GrammarRules::Rule& r : rulesWithNonterminal) {
            if (r == rule) {
                continue;
            }
            GrammarRules::Rule updatedRule = r;
            updatedRule.rightPart.push_back(newNonterminalName);
            newRules.push_back(updatedRule);
            auto pos = std::find(rules.begin(), rules.end(), r);
            if (pos != rules.end()) {
                rules.erase(pos);
            }
        }
        newRules.push_back(newRule);
        newRules.push_back(newEmptyRule);
    }

    static void UpdateRulesWithSamePrefix(std::vector<GrammarRules::Rule>& rules) {
        std::vector<GrammarRules::Rule> newRules;
        std::set<std::string> checkedNonterminals;
        bool hasChanges = false;

        for (size_t i = 0; i < rules.size(); ++i) {
            GrammarRules::Rule& rule = rules[i];
            if (checkedNonterminals.find(rule.nonTerminal) != checkedNonterminals.end()) {
                continue;
            }
            checkedNonterminals.insert(rule.nonTerminal);
            Factorization(rules, rule.nonTerminal, hasChanges, newRules);
        }

        rules = newRules;

        if (hasChanges) {
            UpdateRulesWithSamePrefix(rules);
        }
    }

    static void Factorization(const std::vector<GrammarRules::Rule>& rules, const std::string& nonTerminal, bool& hasChanges, std::vector<GrammarRules::Rule>& newRules) {
        std::vector<GrammarRules::Rule> rulesForNonterminal = GrammarRules::GetRulesByNonTerminal(rules, nonTerminal);
        std::map<std::string, std::vector<GrammarRules::Rule>> symbolToRules;

        for (const GrammarRules::Rule& rule : rulesForNonterminal) {
            symbolToRules[rule.rightPart[0]].push_back(rule);
        }

        if (symbolToRules.size() == rulesForNonterminal.size()) {
            newRules.insert(newRules.end(), rulesForNonterminal.begin(), rulesForNonterminal.end());
            return;
        }

        hasChanges = true;
        for (const auto& entry : symbolToRules) {
            if (entry.second.size() == 1) {
                newRules.push_back(entry.second[0]);
                continue;
            }
            std::vector<std::string> prefix = { entry.first };
            auto prefixAndRules = DefineSamePrefixAndRules(1, prefix, entry.second);
            std::string newNonterminal = GetUniqueNonterminalName(rules, nonTerminal);
            GrammarRules::Rule newRule;
            newRule.nonTerminal = nonTerminal;
            newRule.rightPart = prefixAndRules.first;
            newRule.rightPart.push_back(newNonterminal);
            newRules.push_back(newRule);
            AddNewRulesAfterPrefix(prefixAndRules.second, newNonterminal, prefixAndRules.first.size(), newRules);
        }
    }

    static std::pair<std::vector<std::string>, std::vector<GrammarRules::Rule>> DefineSamePrefixAndRules(
            size_t index, const std::vector<std::string>& prefix, const std::vector<GrammarRules::Rule>& rulesWithPrefix) {
        if (prefix.empty() && index > 0) {
            return { prefix, rulesWithPrefix };
        }
        if (rulesWithPrefix.empty()) {
            return { prefix, rulesWithPrefix };
        }

        std::map<std::string, std::vector<GrammarRules::Rule>> symbolToRules;
        bool hasEnd = false;
        std::string currentSymbol;

        for (const GrammarRules::Rule& rule : rulesWithPrefix) {
            if (index >= rule.rightPart.size()) {
                hasEnd = true;
                continue;
            }
            currentSymbol = rule.rightPart[index];
            symbolToRules[currentSymbol].push_back(rule);
        }

        if (hasEnd || symbolToRules.size() != 1) {
            return { prefix, rulesWithPrefix };
        }

        std::vector<std::string> newPrefix = prefix;
        newPrefix.push_back(currentSymbol);
        return DefineSamePrefixAndRules(index + 1, newPrefix, symbolToRules[currentSymbol]);
    }

    static void AddNewRulesAfterPrefix(
            const std::vector<GrammarRules::Rule>& rulesWithPrefix, const std::string& newNonterminal, size_t beginIndex, std::vector<GrammarRules::Rule>& newRules) {
        for (const GrammarRules::Rule& rule : rulesWithPrefix) {
            GrammarRules::Rule newRule;
            newRule.nonTerminal = newNonterminal;
            newRule.hasEnd = rule.hasEnd;
            if (beginIndex >= rule.rightPart.size()) {
                newRule.rightPart.push_back(GrammarRules::EMPTY_SYMBOL);
            } else {
                for (size_t i = beginIndex; i < rule.rightPart.size(); ++i) {
                    newRule.rightPart.push_back(rule.rightPart[i]);
                }
            }
            newRules.push_back(newRule);
        }
    }
};

#endif // GRAMMAR_PROCESSOR_H