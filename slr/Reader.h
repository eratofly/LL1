#pragma once

#include "Production.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

class Reader
{
public:
	// Чтение правил грамматики из переданного файлового потока
	static std::vector<Production> ReadGrammarRules(std::ifstream& file)
	{
		std::vector<Production> productions;
		std::string line;

		while (std::getline(file, line))
		{
			if (line.empty())
				continue;

			std::istringstream iss(line);
			std::string arrow;
			Production prod;

			iss >> prod.left >> arrow;

			if (arrow != "->")
			{
				std::cerr << "Ошибка формата: ожидалось '->' в правиле: " << line << std::endl;
				continue;
			}

			std::getline(iss, prod.right);
			prod.right.erase(prod.right.begin(), std::ranges::find_if(prod.right, [](int ch) { return !std::isspace(ch); }));

			productions.push_back(prod);
		}

		return productions;
	}
};
//
// int main() {
//    // Пример использования
//    std::ifstream file("grammar.txt");
//    if (!file.is_open()) {
//        std::cerr << "Ошибка: не удалось открыть файл grammar.txt" << std::endl;
//        return 1;
//    }
//
//    auto grammar = Reader::ReadGrammarRules(file);
//    file.close();
//
//    std::cout << "Прочитанные правила:\n";
//    for (const auto& prod : grammar) {
//        std::cout << prod.left << " -> " << prod.right << "\n";
//    }
//
//    return 0;
//}