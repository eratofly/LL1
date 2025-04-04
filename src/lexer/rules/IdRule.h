#pragma once
#include "../reader/Reader.h"

inline bool IsIdChar(char ch)
{
	return ch == '_' || ch == '$' || std::isalpha(ch);
}

/**
 * idChar -> _ | $ | letter
 */
inline bool IdChar(Reader& reader)
{
	return IsIdChar(reader.Get());
}

/**
 * idPart -> idChar | digit
 */
inline bool IdPart(Reader& reader)
{
	if (IdChar(reader))
	{
		return true;
	}
	reader.Unget();

	return std::isdigit(reader.Get());
}

/**
 * simpleIdRemainder -> e | idPart simpleIdRemainder
 */
inline bool SimpleIdRemainder(Reader& reader)
{
	if (reader.Empty())
	{
		return true;
	}

	const auto count = reader.Count();
	if (!IdPart(reader))
	{
		reader.Seek(count);
		return true;
	}

	return SimpleIdRemainder(reader);
}

/**
 * simpleId -> idChar simpleIdRemainder
 */
inline bool SimpleId(Reader& reader)
{
	if (reader.Empty())
	{
		return false;
	}

	return IdChar(reader) && SimpleIdRemainder(reader);
}

inline bool IdRule(Reader& reader);

/**
 * idRemainder -> e | .id
 */
inline bool IdRemainder(Reader& reader)
{
	if (reader.Empty())
	{
		return true;
	}

	if (reader.Peek() == '.')
	{
		reader.Get();
		return IdRule(reader);
	}

	return true;
}

/**
* id -> simpleId idRemainder
 * idRemainder -> e | .id
 * simpleId -> idChar simpleIdRemainder
 * simpleIdRemainder -> e | idPart simpleIdRemainder
 * idPart -> idChar | digit
 * idChar -> _ | $ | letter
 */
inline bool IdRule(Reader& reader)
{
	return SimpleId(reader) && IdRemainder(reader);
}