#pragma once
#include "TableRow.h"
#include <format>
#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>
#include <stack>
#include <unordered_map>
#include <vector>

using Table = std::vector<TableRow>;

class SyntaxAnalyzer
{
public:
	void Analyze(const std::vector<TableRow>& transitionTable, const std::string& fileName)
	{
		ReadTokensFromFile(fileName);

		bool isAnalysisComplete = false;

		while (!isAnalysisComplete)
		{
			const TableRow& currentRow = transitionTable[currentState];
			std::optional<std::string> currentToken = GetCurrentToken();

			const bool isTokenValid = currentToken.has_value() && currentRow.directionSymbols.contains(currentToken.value());

			if (isTokenValid && stateStack.empty() && currentRow.end)
			{
				isAnalysisComplete = true;
				continue;
			}

			if (!isTokenValid && currentRow.error)
			{
				std::string errorMessage = currentToken.value_or("") + " is an incorrect symbol";
				throw std::runtime_error(errorMessage);
			}

			if (!isTokenValid)
			{
				currentState++;
				if (currentRow.shift)
				{
					RemoveCurrentToken();
				}
				continue;
			}

			if (currentRow.stack)
			{
				stateStack.push_back(currentState + 1);
			}

			if (currentRow.pointer.has_value())
			{
				currentState = currentRow.pointer.value();
			}

			if (!currentRow.pointer.has_value())
			{
				currentState = stateStack.back();
				stateStack.pop_back();
			}

			if (currentRow.shift)
			{
				RemoveCurrentToken();
			}
		}
	}

	std::string GetPathway() const
	{
		const char* delim = " -> ";

		std::ostringstream imploded;
		std::copy(pathway.begin(), pathway.end(),
			std::ostream_iterator<size_t>(imploded, delim));

		return "";
	}

	bool Run(std::vector<TableRow> const& transitionTable, std::string const& input)
	{
		stream = std::stringstream(input);
		inputStr = input;
		pathway.clear();
		Shift();
		while (index < transitionTable.size())
		{
			std::cout << index << std::endl;
			pathway.push_back(index);
			auto& [symbol, guidedSet, shift, error, pointer, isStack, end] = transitionTable[index];
			if (!guidedSet.contains(currNonTerminal))
			{
				if (error)
				{
					return false;
				}
				++index;
				continue;
			}

			if (end)
			{
				return stack.empty();
			}

			if (shift)
			{
				Shift();
			}
			if (isStack)
			{
				stack.push(isStack);
			}

			if (pointer.has_value())
			{
				index = pointer.value();
			}
			else if (!stack.empty())
			{
				index = stack.top();
				stack.pop();
			}
			else
			{
				return false;
			}
		}
		return false;
	}

private:
	std::stringstream stream;
	std::string inputStr;
	std::vector<size_t> pathway;
	size_t pos;
	std::string currNonTerminal;
	size_t index = 0;
	TableRow currRow;
	std::unordered_map<int, TableRow> rows;
	std::stack<int> stack;

	std::vector<std::string> tokenList;
	size_t currentState = 0;
	std::vector<size_t> stateStack;

	void Shift()
	{
		stream >> currNonTerminal;
	}

	void ReadTokensFromFile(const std::string& fileName)
	{
		std::ifstream inputFile(fileName);

		if (!inputFile.is_open())
		{
			throw std::runtime_error("Input file not found");
		}

		std::string line;
		while (std::getline(inputFile, line))
		{
			size_t startPos = 0;
			size_t endPos = line.find(' ');
			while (endPos != std::string::npos)
			{
				if (std::string token = line.substr(startPos, endPos - startPos); !token.empty())
				{
					tokenList.push_back(token);
				}
				startPos = endPos + 1;
				endPos = line.find(' ', startPos);
			}
			if (startPos != line.length())
			{
				std::string token = line.substr(startPos);
				tokenList.push_back(token);
			}
		}

		if (tokenList.empty())
		{
			throw std::runtime_error("Input file is empty");
		}
	}

	[[nodiscard]] std::optional<std::string> GetCurrentToken() const
	{
		if (tokenList.empty())
		{
			return std::nullopt;
		}
		return tokenList[0];
	}

	void RemoveCurrentToken()
	{
		if (!tokenList.empty())
		{
			tokenList.erase(tokenList.begin());
		}
	}
};