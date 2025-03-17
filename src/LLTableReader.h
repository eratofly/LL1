#pragma once
#include <fstream>
#include <optional>
#include <sstream>
#include <stack>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

struct TableRow
{
	int num;
	std::string symbol;
	std::unordered_set<std::string> guidedSet;
	bool error;
	std::optional<int> pointer;
	int stack;
	bool shift;
	bool end;
};

class LLTableReader
{
public:
	LLTableReader(std::string fileName)
		: m_fileName(std::move(fileName))
	{
		ParseTable();
	}

	bool Run(std::string const& input)
	{
		m_pathway.clear();
		m_input = input;
		while (true)
		{
			m_pathway.push_back(m_index);
			auto& [num, symbol, guidedSet, error, pointer, stack, shift, end] = m_rows[m_index];
			if (!guidedSet.contains(std::to_string(m_symbol)))
			{
				if (error)
				{
					return false;
				}
				++m_index;
				continue;
			}

			if (end)
			{
				return m_stack.empty();
			}

			if (shift)
			{
				Shift();
			}
			if (stack)
			{
				m_stack.push(stack);
			}

			if (pointer.has_value())
			{
				m_index = pointer.value();
			}
			else if (!m_stack.empty())
			{
				m_index = m_stack.top();
				m_stack.pop();
			}
			else
			{
				return false;
			}
		}
	}

private:
	ParseTable()
	{
		std::ifstream file(m_fileName);
		std::vector<std::vector<std::string>> data;
		std::string line;

		if (!file.is_open())
		{
			throw std::invalid_argument("Could not open file");
		}

		while (getline(file, line))
		{
			std::stringstream ss(line);
			std::string word;
			std::vector<std::string> row;

			while (getline(ss, word, TABLE_SEPARATOR))
			{
				row.push_back(word);
			}

			if (word.empty())
			{
				row.push_back(word);
			}

			data.push_back(row);
		}

		file.close();

		for (auto row : data)
		{
			if (row.size() != TABLE_ROW_ELEMENTS_COUNT)
			{
				throw std::invalid_argument("Wrong number of rows");
			}
			TableRow tableRow{
				std::stoi(row[0]),
				row[1],
				GetGuidedSetFromString(row[2]),
				row[3] == "+",
				row[4].empty() ? std::nullopt : std::stoi(row[4]),
				std::stoi(row[5]),
				row[6] == "+",
				row[7] == "+"
			};
		}
	}

	static std::unordered_set<std::string> GetGuidedSetFromString(std::string const& input)
	{
		std::unordered_set<std::string> guidedSet;
		size_t pos = 0;

		while (true)
		{
			auto index = input.find_first_of(GUIDED_SET_SEPARATOR, pos);
			if (index == std::string::npos)
			{
				return guidedSet;
			}

			guidedSet.emplace(input.substr(pos, index - pos));
			pos = index + GUIDED_SET_SEPARATOR.size();
		}
	}

	void Shift()
	{
		m_symbol = m_input.at(++m_pos);
	}

	static constexpr char TABLE_SEPARATOR = ';';
	static constexpr std::string GUIDED_SET_SEPARATOR = ", ";
	static constexpr int TABLE_ROW_ELEMENTS_COUNT = 8;

	std::string m_fileName;
	int m_pos;
	char m_symbol;
	std::string m_input;
	int m_index = 1;
	TableRow m_currRow;
	std::unordered_map<int, TableRow> m_rows;
	std::stack<int> m_stack;

	std::vector<int> m_pathway;
};