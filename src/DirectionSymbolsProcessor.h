#ifndef DIRECTION_SYMBOLS_PROCESSOR_H
#define DIRECTION_SYMBOLS_PROCESSOR_H

#include "GrammarRules.h"
#include <algorithm>
#include <iostream>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

class DirectionSymbolsProcessor
{
public:
	static std::set<std::string> GetNonTerminalDirectionSymbols(const std::string& nonTerminal, const std::vector<GrammarRules::Rule>& rules)
	{
		std::set<std::string> directionSymbols;
		for (const GrammarRules::Rule& rule : rules)
		{
			if (rule.nonTerminal == nonTerminal)
			{
				directionSymbols.insert(rule.directionSymbols.begin(), rule.directionSymbols.end());
			}
		}
		return directionSymbols;
	}

	static std::set<std::string> GetTerminalsAfterNonTerminal(const GrammarRules::Rule& rule, const std::vector<GrammarRules::Rule>& rules)
	{
		std::vector<GrammarRules::Rule> relevantRules = GrammarRules::GetRulesContainingNonTerminal(rules, rule.nonTerminal);
		std::set<std::string> directionSymbols;
		for (const GrammarRules::Rule& r : relevantRules)
		{
			auto it = std::find(r.rightPart.begin(), r.rightPart.end(), rule.nonTerminal);
			while (it != r.rightPart.end())
			{
				it++;
				if (it == r.rightPart.end())
				{
					continue;
				}
				if (!GrammarRules::IsNonTerminalSymbol(*it, rules))
				{
					directionSymbols.insert(*it);
				}
				it = std::find(it, r.rightPart.end(), rule.nonTerminal);
			}
		}
		return directionSymbols;
	}

	static std::set<std::string> GetDirectionSymbolsAfterNonTerminal(const GrammarRules::Rule& rule, const std::string& nonTerminal, const std::vector<GrammarRules::Rule>& rules)
	{
		std::set<std::string> directionSymbols;
		auto it = std::find(rule.rightPart.begin(), rule.rightPart.end(), nonTerminal);
		while (it != rule.rightPart.end())
		{
			it++;
			if (it == rule.rightPart.end())
			{
				std::set<std::string> symbols = GetDirectionSymbolsAtEnd(rule, rules);
				directionSymbols.insert(symbols.begin(), symbols.end());
				continue;
			}
			if (GrammarRules::IsNonTerminalSymbol(*it, rules))
			{
				std::set<std::string> symbols = GetNonTerminalDirectionSymbols(*it, rules);
				directionSymbols.insert(symbols.begin(), symbols.end());
			}
			else
			{
				directionSymbols.insert(*it);
			}
			it = std::find(it, rule.rightPart.end(), nonTerminal);
		}
		return directionSymbols;
	}

	static std::set<std::string> GetDirectionSymbolsAtEnd(const GrammarRules::Rule& rule, const std::vector<GrammarRules::Rule>& rules)
	{
		std::set<std::string> directionSymbols;
		std::vector<GrammarRules::Rule> relevantRules = GrammarRules::GetRulesContainingNonTerminal(rules, rule.nonTerminal);
		if (rule.hasEnd)
		{
			directionSymbols.insert(GrammarRules::END_SYMBOL);
		}
		for (const GrammarRules::Rule& r : relevantRules)
		{
			if (std::find(rule.rightPart.begin(), rule.rightPart.end(), r.nonTerminal) == rule.rightPart.end())
			{
				std::set<std::string> symbols = GetDirectionSymbolsAfterNonTerminal(r, rule.nonTerminal, rules);
				directionSymbols.insert(symbols.begin(), symbols.end());
			}
		}
		return directionSymbols;
	}

	static void ProcessDirectionSymbols(std::vector<GrammarRules::Rule>& rules)
	{
		bool hasChanges = false;
		for (size_t i = 0; i < rules.size(); i++)
		{
			GrammarRules::Rule& rule = rules[i];
			if (rule.rightPart.empty())
			{
				throw std::logic_error("Right part is empty");
			}
			if (rule.rightPart.size() == 1 && rule.rightPart[0] == GrammarRules::EMPTY_SYMBOL)
			{
				std::set<std::string> directionSymbols = GetDirectionSymbolsAfterNonTerminalInGrammar(rule.nonTerminal, rules);
				size_t sizeBefore = rule.directionSymbols.size();
				rule.directionSymbols.insert(directionSymbols.begin(), directionSymbols.end());
				hasChanges = !(sizeBefore == rule.directionSymbols.size() && !hasChanges);
				continue;
			}
			if (GrammarRules::IsNonTerminalSymbol(rule.rightPart[0], rules))
			{
				std::set<std::string> directionSymbols = GetNonTerminalDirectionSymbols(rule.rightPart[0], rules);
				size_t sizeBefore = rule.directionSymbols.size();
				rule.directionSymbols.insert(directionSymbols.begin(), directionSymbols.end());
				hasChanges = !(sizeBefore == rule.directionSymbols.size() && !hasChanges);
				continue;
			}
			size_t sizeBefore = rule.directionSymbols.size();
			rules[i].directionSymbols.insert(rule.rightPart[0]);
			hasChanges = !(sizeBefore == rule.directionSymbols.size() && !hasChanges);
		}

		if (hasChanges)
		{
			ProcessDirectionSymbols(rules);
		}
	}

	static std::set<std::string> GetDirectionSymbolsAfterNonTerminalInGrammar(const std::string& nonTerminal, const std::vector<GrammarRules::Rule>& rules)
	{
		std::set<std::string> directionSymbols;
		for (const GrammarRules::Rule& rule : rules)
		{
			std::set<std::string> symbols = GetDirectionSymbolsAfterNonTerminal(rule, nonTerminal, rules);
			directionSymbols.insert(symbols.begin(), symbols.end());
		}
		return directionSymbols;
	}
};

#endif // DIRECTION_SYMBOLS_PROCESSOR_H