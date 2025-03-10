#include <catch.hpp>
#include "../src/lexer/Lexer.h"

template <typename... Args>
void Check(Lexer lexer, Args&&... args)
{
	auto checkOne = [&lexer](auto token) {
		const auto [type, value, pos, error] = lexer.Get();
		CHECK(type == token.type);
		CHECK(value == token.value);
		CHECK(pos == token.pos);
		CHECK(error == token.error);
	};
	(checkOne(args), ...);
}

TEST_CASE("peek token")
{
	Lexer lexer("first second");
	CHECK(lexer.Peek() == Token{ TokenType::ID, "first", 0});
	CHECK(lexer.Peek() == Token{ TokenType::ID, "first", 0});
	CHECK(lexer.Get() == Token{ TokenType::ID, "first", 0});
	CHECK(lexer.Peek() == Token{ TokenType::ID, "second", 6});
	CHECK(lexer.Get() == Token{ TokenType::ID, "second", 6});
}

TEST_CASE("empty lexer")
{
	Check(Lexer(""),
		Token{ TokenType::ERROR, "", 0, Error::EMPTY_INPUT }
		);

	Check(Lexer("    "),
		Token{ TokenType::ERROR, "", 0, Error::EMPTY_INPUT }
		);
}

TEST_CASE("valid id tests")
{

	Check(Lexer("id secondId    m_id a _$s123 plus+"),
		Token{ TokenType::ID, "id", 0 },
		Token{ TokenType::ID, "secondId", 3 },
		Token{ TokenType::ID, "m_id", 15 },
		Token{ TokenType::ID, "a", 20 },
		Token{ TokenType::ID, "_$s123", 22 },
		Token{ TokenType::ID, "plus", 29 }
		);
}

TEST_CASE("invalid id tests")
{
	Check(Lexer("obj.id.field error."),
		Token{ TokenType::ID, "obj.id.field", 0 },
		Token{ TokenType::ERROR, "", 13, Error::INVALID_ID }
		);
}


TEST_CASE("valid numbers tests")
{
	Check(Lexer("5 0 123 12.52 0.25 0.0013 1e+10 100.53E-15"),
		Token{ TokenType::INTEGER, "5", 0 },
		Token{ TokenType::INTEGER, "0", 2 },
		Token{ TokenType::INTEGER, "123", 4 },
		Token{ TokenType::FLOAT, "12.52", 8 },
		Token{ TokenType::FLOAT, "0.25", 14 },
		Token{ TokenType::FLOAT, "0.0013", 19 },
		Token{ TokenType::FLOAT, "1e+10", 26 },
		Token{ TokenType::FLOAT, "100.53E-15", 32 }
		);
}

TEST_CASE("invalid numbers tests")
{
	Check(Lexer("05"), Token{ TokenType::ERROR, "", 0, Error::INVALID_NUMBER });
	Check(Lexer("00.5"), Token{ TokenType::ERROR, "", 0, Error::INVALID_NUMBER });
	Check(Lexer("12e+"), Token{ TokenType::ERROR, "", 0, Error::INVALID_NUMBER });
	Check(Lexer("12.e+10"), Token{ TokenType::ERROR, "", 0, Error::INVALID_NUMBER });
	Check(Lexer("42.53e"), Token{ TokenType::ERROR, "", 0, Error::INVALID_NUMBER });
	Check(Lexer(" 42.53e10"), Token{ TokenType::ERROR, "", 1, Error::INVALID_NUMBER });
	Check(Lexer("  42.53e+0"), Token{ TokenType::ERROR, "", 2, Error::INVALID_NUMBER });
}

TEST_CASE("valid string tests")
{
	Check(Lexer("'' 'Hello, World!'"),
		Token{ TokenType::STRING_LITERAL, "''", 0 },
		Token{ TokenType::STRING_LITERAL, "'Hello, World!'", 3 }
		);
}

TEST_CASE("invalid string tests")
{
	Check(Lexer("'"), Token{ TokenType::ERROR, "", 0, Error::STRING_LITERAL_INCOMPLETE });
	Check(Lexer("   'Hello"), Token{ TokenType::ERROR, "", 3, Error::STRING_LITERAL_INCOMPLETE });
}

TEST_CASE("reserved words tests")
{
	Check(Lexer("mod div and or not true false"),
		Token{ TokenType::OP_MOD, "mod", 0 },
		Token{ TokenType::OP_DIV, "div", 4 },
		Token{ TokenType::OP_AND, "and", 8 },
		Token{ TokenType::OP_OR, "or", 12 },
		Token{ TokenType::OP_NOT, "not", 15 },
		Token{ TokenType::TRUE, "true", 19 },
		Token{ TokenType::FALSE, "false", 24 }
		);

	Check(Lexer("nott"), Token{ TokenType::ID, "nott", 0 });
}

TEST_CASE("valid special chars tests")
{
	Check(Lexer("(){}[],+-*/===<><=>=!=!"),
		Token{ TokenType::PARAN_OPEN, "(", 0 },
		Token{ TokenType::PARAN_CLOSE, ")", 1 },
		Token{ TokenType::CURLY_OPEN, "{", 2 },
		Token{ TokenType::CURLY_CLOSE, "}", 3 },
		Token{ TokenType::BRACKET_OPEN, "[", 4 },
		Token{ TokenType::BRACKET_CLOSE, "]", 5 },
		Token{ TokenType::COMMA, ",", 6 },
		Token{ TokenType::OP_PLUS, "+", 7 },
		Token{ TokenType::OP_MINUS, "-", 8 },
		Token{ TokenType::OP_MUL, "*", 9 },
		Token{ TokenType::OP_DIVISION, "/", 10 },
		Token{ TokenType::OP_EQUAL, "==", 11 },
		Token{ TokenType::OP_ASSIGNMENT, "=", 13 },
		Token{ TokenType::OP_LESS, "<", 14 },
		Token{ TokenType::OP_GREATER, ">", 15 },
		Token{ TokenType::OP_LESS_OR_EQUAL, "<=", 16 },
		Token{ TokenType::OP_GREATER_OR_EQUAL, ">=", 18 },
		Token{ TokenType::OP_NOT_EQUAL, "!=", 20 },
		Token{ TokenType::OP_NOT_MARK, "!", 22 }
		);
}

TEST_CASE("invalid special chars tests")
{
	Check(Lexer("#"), Token{ TokenType::ERROR, "#", 0, Error::UNKNOWN_SYMBOL });
	Check(Lexer("@"), Token{ TokenType::ERROR, "@", 0, Error::UNKNOWN_SYMBOL });
}

TEST_CASE("complex expressions")
{
	Check(Lexer("-a + 5.3E-15 * (-a + -b * (a * -b) -c) != abc"),
		Token{ TokenType::OP_MINUS, "-", 0 },
		Token{ TokenType::ID, "a", 1 },
		Token{ TokenType::OP_PLUS, "+", 3 },
		Token{ TokenType::FLOAT, "5.3E-15", 5 },
		Token{ TokenType::OP_MUL, "*", 13 },
		Token{ TokenType::PARAN_OPEN, "(", 15 },
		Token{ TokenType::OP_MINUS, "-", 16 },
		Token{ TokenType::ID, "a", 17 },
		Token{ TokenType::OP_PLUS, "+", 19 },
		Token{ TokenType::OP_MINUS, "-", 21 },
		Token{ TokenType::ID, "b", 22 },
		Token{ TokenType::OP_MUL, "*", 24 },
		Token{ TokenType::PARAN_OPEN, "(", 26 },
		Token{ TokenType::ID, "a", 27 },
		Token{ TokenType::OP_MUL, "*", 29 },
		Token{ TokenType::OP_MINUS, "-", 31 },
		Token{ TokenType::ID, "b", 32 },
		Token{ TokenType::PARAN_CLOSE, ")", 33 },
		Token{ TokenType::OP_MINUS, "-", 35 },
		Token{ TokenType::ID, "c", 36 },
		Token{ TokenType::PARAN_CLOSE, ")", 37 },
		Token{ TokenType::OP_NOT_EQUAL, "!=", 39 },
		Token{ TokenType::ID, "abc", 42 }
		);

	Check(Lexer("5 + 10 * 3.14 - 'test' / 2.5"),
		Token{ TokenType::INTEGER, "5", 0 },
		Token{ TokenType::OP_PLUS, "+", 2 },
		Token{ TokenType::INTEGER, "10", 4 },
		Token{ TokenType::OP_MUL, "*", 7 },
		Token{ TokenType::FLOAT, "3.14", 9 },
		Token{ TokenType::OP_MINUS, "-", 14 },
		Token{ TokenType::STRING_LITERAL, "'test'", 16 },
		Token{ TokenType::OP_DIVISION, "/", 23 },
		Token{ TokenType::FLOAT, "2.5", 25 }
		);

	Check(Lexer("true and false or x > 10"),
		Token{ TokenType::TRUE, "true", 0 },
		Token{ TokenType::OP_AND, "and", 5 },
		Token{ TokenType::FALSE, "false", 9 },
		Token{ TokenType::OP_OR, "or", 15 },
		Token{ TokenType::ID, "x", 18 },
		Token{ TokenType::OP_GREATER, ">", 20 },
		Token{ TokenType::INTEGER, "10", 22 }
		);

	Check(Lexer("((5 + 2) * (3 - 1)) / 4"),
		Token{ TokenType::PARAN_OPEN, "(", 0 },
		Token{ TokenType::PARAN_OPEN, "(", 1 },
		Token{ TokenType::INTEGER, "5", 2 },
		Token{ TokenType::OP_PLUS, "+", 4 },
		Token{ TokenType::INTEGER, "2", 6 },
		Token{ TokenType::PARAN_CLOSE, ")", 7 },
		Token{ TokenType::OP_MUL, "*", 9 },
		Token{ TokenType::PARAN_OPEN, "(", 11 },
		Token{ TokenType::INTEGER, "3", 12 },
		Token{ TokenType::OP_MINUS, "-", 14 },
		Token{ TokenType::INTEGER, "1", 16 },
		Token{ TokenType::PARAN_CLOSE, ")", 17 },
		Token{ TokenType::PARAN_CLOSE, ")", 18 },
		Token{ TokenType::OP_DIVISION, "/", 20 },
		Token{ TokenType::INTEGER, "4", 22 }
		);
}