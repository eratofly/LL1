#pragma once
#include <vector>
#include <string>
#include <algorithm>

class StringHandler {
public:
    static std::vector<std::string> Split(const std::string& str, const std::string& separator) {
        std::vector<std::string> strs;
        size_t startPos = 0;
        size_t endPos = str.find(separator);

        while (endPos != std::string::npos) {
            std::string partOfStr = str.substr(startPos, endPos - startPos);
            if (!partOfStr.empty()) {
                strs.push_back(partOfStr);
            }
            startPos = endPos + separator.length();
            endPos = str.find(separator, startPos);
        }

        if (startPos != str.length()) {
            std::string partOfStr = str.substr(startPos);
            if (!partOfStr.empty()) {
                strs.push_back(partOfStr);
            }
        }

        return strs;
    }

    static std::string RemoveSpacesInBeginAndEndOfWord(const std::string& str) {
        size_t start = str.find_first_not_of(" \t\n\r");
        size_t end = str.find_last_not_of(" \t\n\r");

        if (start == std::string::npos || end == std::string::npos) {
            return "";
        }

        return str.substr(start, end - start + 1);
    }
};