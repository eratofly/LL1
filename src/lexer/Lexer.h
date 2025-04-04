#pragma once
#include "Token.h"
#include "reader/Reader.h"
#include "rules/IdRule.h"
#include "rules/NumberRule.h"
#include "rules/ReservedWordsRule.h"
#include "rules/SpecialCharsRule.h"
#include "rules/StringRule.h"

class Lexer
{
public:
	Lexer(std::string const& input)
		: m_reader(input)
	{
	}

	Token Get()
	{
		SkipWhitespaces();
		if (Empty())
		{
			return Token{
				.type = TokenType::ERROR,
				.pos = m_reader.Count(),
				.error = Error::EMPTY_INPUT
			};
		}

		const auto ch = m_reader.Peek();

		if (IsIdChar(ch))
		{
			return CheckReserved(Id());
		}
		if (Digit(ch))
		{
			return Number();
		}
		if (IsQuot(ch))
		{
			return String();
		}
		if (IsSpecialChar(ch))
		{
			return SpecialChar();
		}

		return Token{
			.type = TokenType::ERROR,
			.value = std::string{ ch },
			.pos = m_reader.Count(),
			.error = Error::UNKNOWN_SYMBOL,
		};
	}

	Token Peek()
	{
		const auto pos = m_reader.Count();
		const auto token = Get();
		m_reader.Seek(pos);
		return token;
	}

	[[nodiscard]] bool Empty()
	{
		SkipWhitespaces();
		return m_reader.Empty();
	}

private:
	Token Id()
	{
		const auto startPos = m_reader.Count();
		m_reader.Record();
		if (!IdRule(m_reader))
		{
			return Token{
				.type = TokenType::ERROR,
				.pos = startPos,
				.error = Error::INVALID_ID,
			};
		}

		return Token{
			.type = TokenType::ID,
			.value = m_reader.StopRecord(),
			.pos = startPos,
		};
	}

	Token Number()
	{
		const auto startPos = m_reader.Count();
		m_reader.Record();
		bool isInteger = false;
		if (!NumberRule(m_reader, isInteger))
		{
			return Token{
				.type = TokenType::ERROR,
				.pos = startPos,
				.error = Error::INVALID_NUMBER,
			};
		}

		return Token{
			.type = isInteger ? TokenType::INTEGER : TokenType::FLOAT,
			.value = m_reader.StopRecord(),
			.pos = startPos,
		};
	}

	Token String()
	{
		const auto startPos = m_reader.Count();
		m_reader.Record();
		if (!StringRule(m_reader))
		{
			return Token{
				.type = TokenType::ERROR,
				.pos = startPos,
				.error = Error::STRING_LITERAL_INCOMPLETE,
			};
		}

		return Token{
			.type = TokenType::STRING_LITERAL,
			.value = m_reader.StopRecord(),
			.pos = startPos,
		};
	}

	Token SpecialChar()
	{
		const auto startPos = m_reader.Count();
		m_reader.Record();
		const auto tokenType = SpecialCharRule(m_reader);
		return Token{
			.type = tokenType,
			.value = m_reader.StopRecord(),
			.pos = startPos,
		};
	}

	void SkipWhitespaces()
	{
		while (!m_reader.Empty() && m_reader.Peek() == ' ')
		{
			m_reader.Get();
		}
	}

private:
	Reader m_reader;
};