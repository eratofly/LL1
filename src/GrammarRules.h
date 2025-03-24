#pragma once
#include <algorithm>
#include <optional>
#include <set>
#include <string>
#include <vector>

class GrammarRules
{
public:
	constexpr static const std::string EMPTY_SYMBOL = "e";
	constexpr static const std::string END_SYMBOL = "#";

	struct Rule
	{
		std::string nonTerminal;
		std::vector<std::string> rightPart;
		std::set<std::string> directionSymbols;
		bool hasEnd = false;

		bool operator==(const Rule& other) const
		{
			return nonTerminal == other.nonTerminal && rightPart == other.rightPart && directionSymbols == other.directionSymbols;
		}
	};

	static bool IsNonTerminalSymbol(const std::string& symbol, const std::vector<Rule>& rules)
	{
		for (const Rule& rule : rules)
		{
			if (rule.nonTerminal == symbol)
			{
				return true;
			}
		}
		return false;
	}

	static std::optional<size_t> FindNonTerminalIndex(const std::vector<Rule>& rules, const std::string& nonTerminal)
	{
		for (size_t i = 0; i < rules.size(); ++i)
		{
			if (rules[i].nonTerminal == nonTerminal)
			{
				return i;
			}
		}
		return std::nullopt;
	}

	static std::vector<Rule> GetRulesContainingNonTerminal(const std::vector<Rule>& rules, const std::string& nonTerminal)
	{
		std::vector<Rule> matchingRules;
		for (const Rule& rule : rules)
		{
			if (std::find(rule.rightPart.begin(), rule.rightPart.end(), nonTerminal) != rule.rightPart.end())
			{
				matchingRules.push_back(rule);
			}
		}
		return matchingRules;
	}

	static std::vector<Rule> GetRulesByNonTerminal(const std::vector<Rule>& rules, const std::string& nonTerminal)
	{
		std::vector<Rule> matchingRules;
		for (const Rule& rule : rules)
		{
			if (rule.nonTerminal == nonTerminal)
			{
				matchingRules.push_back(rule);
			}
		}
		return matchingRules;
	}
};