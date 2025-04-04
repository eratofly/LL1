#pragma once
#include <format>
#include <fstream>
#include <iostream>
#include <stdexcept>

#include "../lexer/Lexer.h"
#include "StringHelpers.h"
#include "TableRow.h"

#include <stack>
#include <utility>

struct ErrorReason
{
	std::set<std::string> expected;
	Token received;
	bool operator==(const ErrorReason&) const = default;
};

class LLParser
{
public:
	explicit LLParser(Table table)
		: m_table(std::move(table))
	{}

	bool Parse(std::string const& input)
	{
		m_lexer = Lexer(input);
		m_index = 0;
		m_row = m_table[0];
		m_error = {};
		Shift();

		while (true)
		{
			const auto& [symbol, guides, shift, error, ptr, stack, end] = m_row;
			if (!guides.contains(m_symbol))
			{
				if (error)
				{
					RecordError();
					return false;
				}
				++m_index;
				m_row = m_table.at(m_index);
				continue;
			}

			if (end)
			{
				return true;
			}

			if (shift)
			{
				Shift();
			}

			if (stack)
			{
				m_stack.push(m_index + 1);
			}

			if (ptr.has_value())
			{
				m_index = ptr.value();
			}
			else
			{
				m_index = m_stack.top();
				m_stack.pop();
			}
			m_row = m_table.at(m_index);
		}
	}

	ErrorReason GetError() const { return m_error; }

private:
	void Shift()
	{
		auto token = m_lexer.Get();
		RecordToken(token);
		m_symbol = token.error == Error::EMPTY_INPUT
			? END_SYMBOL
			: RemapTokenTypeToString(token.type);
	}

	Token Peek()
	{
		auto token = m_lexer.Peek();
		RecordToken(token);
		return token;
	}

	bool Empty() { return m_lexer.Empty(); }

	void RecordToken(Token const& token)
	{
		m_lastToken = token;
	}

	void RecordError()
	{
		m_error = {
			.expected = m_row.directionSymbols,
			.received = m_lastToken.value(),
		};
	}

	static inline const std::string END_SYMBOL = "#";

	Lexer m_lexer{ "" };
	Table m_table{};
	std::optional<Token> m_lastToken;
	ErrorReason m_error{};
	std::string m_symbol;
	TableRow m_row;
	size_t m_index = 0;
	std::stack<size_t> m_stack;
};