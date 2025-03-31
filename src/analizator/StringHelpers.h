#pragma once
#include <vector>
#include <string>
#include <algorithm>

std::vector<std::string> Split(const std::string& str, const std::string& separator);

inline void LTrim(std::string& s);

inline void rTrim(std::string& s);

std::string RemoveSpacesInBeginAndEndOfWord(const std::string& str);