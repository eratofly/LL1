#include "../src/errors/Error.h"
#include "../src/parser/Parser.h"

#include <catch.hpp>

void CheckError(std::string const& str, Error error, size_t pos)
{
	Parser parser(str);
	CHECK_FALSE(parser.Parse());
	CHECK(parser.GetError() == error);
	CHECK(pos == parser.GetLastToken()->pos);
}

TEST_CASE("parser error tests")
{
	CheckError("5 * * 5", Error::TERM_EXPECTED, 4);
	CheckError("a + (5 * (id - 7)", Error::PARAN_CLOSE_EXPECTED, 16);
}