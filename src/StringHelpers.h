#pragma once
#include <vector>
#include <string>
#include <algorithm>

std::vector<std::string> Split(const std::string& str, const std::string& separator);

std::string RemoveSpacesInBeginAndEndOfWord(const std::string& str);