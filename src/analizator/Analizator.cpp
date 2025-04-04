#include "Analizator.h"
#include "../lexer/Lexer.h"

std::vector<std::string> tokens;

void GenerateTokensFromFile(const std::string& inputFileName)
{
	std::ifstream input(inputFileName);

	if (!input.is_open())
	{
		throw std::runtime_error("Input file is not found");
	}

	std::string str;

	while (std::getline(input, str))
	{
		std::vector<std::string> tokensOfStr = Split(str, " ");
		tokens.insert(tokens.end(), tokensOfStr.begin(), tokensOfStr.end());
	}

	if (tokens.empty())
	{
		throw std::runtime_error("Input file is empty");
	}
}

std::optional<std::string> ReadToken()
{
	if (tokens.empty())
	{
		return std::nullopt;
	}

	return tokens[0];
}

void PopToken()
{
	if (!tokens.empty())
	{
		tokens.erase(tokens.begin());
	}
}

void Analyze(const Table table, const std::string& inputFileName)
{
	GenerateTokensFromFile(inputFileName);

	int pos = 0;
	size_t current = 0;
	std::vector<size_t> stack;
	bool isAnalyzed = true;
	while (isAnalyzed)
	{
		++pos;
		TableRow state = table[current];
		bool isDirectionSymbol = std::ranges::find(state.directionSymbols, ReadToken().value()) != state.directionSymbols.end();
		if (isDirectionSymbol && stack.empty() && state.end)
		{
			isAnalyzed = false;
			continue;
		}
		if (!isDirectionSymbol && state.error)
		{
			std::string msg = ReadToken().value() + " is incorrect symbol in position " + std::to_string(pos - 1);
			throw std::runtime_error(msg);
		}
		if (!isDirectionSymbol)
		{
			current++;
			if (state.shift)
			{
				PopToken();
			}
			continue;
		}
		if (state.stack)
		{
			stack.push_back(current + 1);
		}
		if (state.pointer.has_value())
		{
			current = state.pointer.value();
		}
		if (!state.pointer.has_value())
		{
			current = stack[stack.size() - 1];
			stack.pop_back();
		}
		if (state.shift)
		{
			PopToken();
		}
	}
}