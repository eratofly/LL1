#define CATCH_CONFIG_MAIN
#include <catch.hpp>
#include "../src/lexer/reader/Reader.h"

SCENARIO("Reader reads characters and tracks position")
{
	GIVEN("A Reader initialized with a string stream containing 'Hello, World!'")
	{
		Reader reader("Hello, World!");

		WHEN("We read the first character")
		{
			reader.Record();
			char ch = reader.Get();

			THEN("The character should be 'H'")
			{
				CHECK(ch == 'H');
				CHECK(reader.StopRecord() == "H");
			}

			AND_THEN("The position should be 1")
			{
				CHECK(reader.Count() == 1);
			}

			AND_THEN("The stream should not be empty")
			{
				CHECK_FALSE(reader.Empty());
			}
		}

		WHEN("We peek at the first character without advancing the reader")
		{
			reader.Record();
			char c = reader.Peek();

			THEN("The character should be 'H'")
			{
				CHECK(reader.StopRecord().empty());
				CHECK(c == 'H');
			}

			AND_THEN("The character should still be 'H' and position should be 0")
			{
				CHECK(c == 'H');
				REQUIRE(reader.Count() == 0);
			}
		}

		WHEN("We read all characters from the stream")
		{
			reader.Record();
			reader.Get(); // 'H'
			reader.Get(); // 'e'
			reader.Get(); // 'l'
			reader.Get(); // 'l'
			reader.Get(); // 'o'
			CHECK(reader.StopRecord() == "Hello");

			reader.Record();
			reader.Get(); // ','
			reader.Get(); // ' '
			reader.Get(); // 'W'
			reader.Get(); // 'o'
			reader.Get(); // 'r'
			reader.Get(); // 'l'
			reader.Get(); // 'd'
			reader.Get(); // '!'
			CHECK(reader.StopRecord() == ", World!");

			THEN("The stream should be empty")
			{
				CHECK(reader.Empty());
			}

			AND_THEN("The position should be 13")
			{
				CHECK(reader.Count() == 13);
			}
		}

		WHEN("We try to read from an empty stream")
		{
			reader.Get(); // 'H'
			reader.Get(); // 'e'
			reader.Get(); // 'l'
			reader.Get(); // 'l'
			reader.Get(); // 'o'
			reader.Get(); // ','
			reader.Get(); // ' '
			reader.Get(); // 'W'
			reader.Get(); // 'o'
			reader.Get(); // 'r'
			reader.Get(); // 'l'
			reader.Get(); // 'd'
			reader.Get(); // '!'

			THEN("Trying to read should throw an exception")
			{
				CHECK_THROWS_AS(reader.Get(), std::runtime_error);
			}
		}

		WHEN("We unget a character")
		{
			reader.Record();
			reader.Get();
			reader.Unget();

			THEN("The position should be 0")
			{
				CHECK(reader.StopRecord().empty());
				CHECK(reader.Count() == 0);
			}

			AND_THEN("The next character read should be 'H' again")
			{
				CHECK(reader.Get() == 'H');
			}
		}

		WHEN("Seek reader")
		{
			reader.Record();
			reader.Get(); // 'H'
			reader.Get(); // 'e'
			reader.Get(); // 'l'
			reader.Get(); // 'l'
			reader.Get(); // 'o'

			reader.Seek(1);

			THEN("The position should be 1")
			{
				CHECK(reader.StopRecord() == "H");
				CHECK(reader.Count() == 1);
				CHECK(reader.Peek() == 'e');
			}
		}
	}
}

