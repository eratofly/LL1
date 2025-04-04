#pragma once
#include <string>
#include <unordered_map>

enum class Error
{
	NONE,
	UNKNOWN_SYMBOL,
	INVALID_NUMBER,
	STRING_LITERAL_INCOMPLETE,
	EMPTY_INPUT,
	INVALID_ID,
	TERM_EXPECTED,
	PARAN_CLOSE_EXPECTED,
};

enum class TokenType
{
	ERROR,
	PARAN_OPEN, // (
	PARAN_CLOSE, // )
	CURLY_OPEN, // {
	CURLY_CLOSE, // }
	BRACKET_OPEN, // [
	BRACKET_CLOSE, // ]
	COMMA, // ,
	OP_PLUS, // +
	OP_MINUS, // -
	OP_MUL, // *
	OP_MOD, // mod
	OP_DIV, // div
	OP_DIVISION, // /
	OP_ASSIGNMENT, // =
	OP_EQUAL, // ==
	OP_NOT_EQUAL, // !=
	OP_LESS, // <
	OP_GREATER, // >
	OP_LESS_OR_EQUAL, // <=
	OP_GREATER_OR_EQUAL, // >=
	OP_AND, // and
	OP_OR, // or
	OP_NOT, // not
	OP_NOT_MARK, // !
	ID, // identifier
	INTEGER, // integer
	FLOAT, // float
	STRING_LITERAL, // string literal
	TRUE, // true
	FALSE, // false
	IF, // if
	SEMICOLON, // ;
	THEN, // then
	ELSE, // else
};

struct Token
{
	TokenType type;
	std::string value;
	size_t pos;
	Error error = Error::NONE;
};

inline std::unordered_map<TokenType, std::string> TokenTypeToString = {
	{ TokenType::ERROR, "error" },
	{ TokenType::PARAN_OPEN, "(" },
	{ TokenType::PARAN_CLOSE, ")" },
	{ TokenType::CURLY_OPEN, "{" },
	{ TokenType::CURLY_CLOSE, "}" },
	{ TokenType::BRACKET_OPEN, "[" },
	{ TokenType::BRACKET_CLOSE, "]" },
	{ TokenType::COMMA, "," },
	{ TokenType::OP_PLUS, "+" },
	{ TokenType::OP_MINUS, "-" },
	{ TokenType::OP_MUL, "*" },
	{ TokenType::OP_MOD, "mod" },
	{ TokenType::OP_DIV, "div" },
	{ TokenType::OP_DIVISION, "/" },
	{ TokenType::OP_ASSIGNMENT, "=" },
	{ TokenType::OP_EQUAL, "==" },
	{ TokenType::OP_NOT_EQUAL, "!=" },
	{ TokenType::OP_LESS, "<" },
	{ TokenType::OP_GREATER, ">" },
	{ TokenType::OP_LESS_OR_EQUAL, "<=" },
	{ TokenType::OP_GREATER_OR_EQUAL, ">=" },
	{ TokenType::OP_AND, "and" },
	{ TokenType::OP_OR, "or" },
	{ TokenType::OP_NOT, "not" },
	{ TokenType::OP_NOT_MARK, "!" },
	{ TokenType::ID, "id" },
	{ TokenType::INTEGER, "int" },
	{ TokenType::FLOAT, "float" },
	{ TokenType::STRING_LITERAL, "string" },
	{ TokenType::TRUE, "true" },
	{ TokenType::FALSE, "false" },
	{ TokenType::IF, "if" },
	{ TokenType::SEMICOLON, ";" },
	{ TokenType::THEN, "then" },
	{ TokenType::ELSE, "else" },
};

inline std::string RemapTokenTypeToString(const TokenType type)
{
	return TokenTypeToString.at(type);
}

inline bool operator==(Token const& left, Token const& right)
{
	return left.type == right.type && left.value == right.value && left.pos == right.pos && left.error == right.error;
}