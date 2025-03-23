#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <algorithm>

using namespace std;

// Структура для хранения информации о знаке
struct Sign {
    size_t index;               // Порядковый номер
    string value;               // Значение (терминал или нетерминал)
    vector<string> directionSymbols; // Направляющие символы
    bool isTerminal;            // true для терминала, false для нетерминала
    string part;                // Часть правила: "left" или "right"
    vector<pair<size_t, string>> leftPartPairs; // Пары (индекс, значение) для правой части

    // Конструктор для удобства
    Sign(size_t idx, const string& val, const vector<string>& symbols, bool isTerm, const string& p)
            : index(idx), value(val), directionSymbols(symbols), isTerminal(isTerm), part(p) {}

    // Для удобства вывода
    friend ostream& operator<<(ostream& os, const Sign& sign) {
        os << "Index: " << sign.index
           << ", Value: " << sign.value
           << ", Direction Symbols: [";
        for (const auto& sym : sign.directionSymbols) {
            os << sym << " ";
        }
        os << "], IsTerminal: " << (sign.isTerminal ? "Yes" : "No")
           << ", Part: " << sign.part
           << ", Left Part Pairs: [";
        for (const auto& pair : sign.leftPartPairs) {
            os << "(" << pair.first << ", " << pair.second << ") ";
        }
        os << "]";
        return os;
    }
};

// Функция для удаления пробелов в начале и конце строки
string trim(const string& str) {
    size_t first = str.find_first_not_of(' ');
    if (first == string::npos) return "";
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, last - first + 1);
}

// Функция для поиска слов между <>
vector<string> extractNonTerminals(const string& line) {
    vector<string> nonTerminals;
    size_t start = 0;
    while ((start = line.find('<', start)) != string::npos) {
        size_t end = line.find('>', start);
        if (end == string::npos) break;
        nonTerminals.push_back(line.substr(start + 1, end - start - 1));
        start = end + 1;
    }
    return nonTerminals;
}

// Функция для разбиения строки на терминалы и нетерминалы
vector<string> splitIntoTerminalsAndNonTerminals(const string& str) {
    vector<string> result;
    size_t i = 0;
    while (i < str.size()) {
        if (str[i] == '<') {
            // Нетерминал: начинается с '<' и заканчивается '>'
            size_t end = str.find('>', i);
            if (end == string::npos) break; // Если нет закрывающей скобки, выходим
            result.push_back(str.substr(i, end - i + 1)); // Добавляем нетерминал с <>
            i = end + 1;
        } else if (str[i] == '\'') {
            // Апостроф: пропускаем, так как это разделительный знак
            i++;
            // Собираем все символы после апострофа в один токен
            string token;
            while (i < str.size() && str[i] != '\'' && str[i] != '<' && str[i] != ' ') {
                token += str[i];
                i++;
            }
            if (!token.empty()) {
                result.push_back(token);
            }
        } else {
            // Терминал: одиночный символ
            result.push_back(string(1, str[i])); // Добавляем терминал
            i++;
        }
    }
    return result;
}

// Функция для чтения файла и заполнения ruleMap, afterSlashMap и nonTerminals
void readGrammarFile(const string& filename, vector<pair<string, string>>& ruleMap,
                     vector<pair<string, vector<string>>>& afterSlashMap, vector<string>& nonTerminals) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Ошибка открытия файла!" << endl;
        return;
    }

    string line;
    while (getline(file, line)) {
        line = trim(line);
        if (line.empty()) continue; // Пропускаем пустые строки

        // Разделяем строку на части по ->
        size_t arrowPos = line.find("->");
        if (arrowPos == string::npos) continue; // Пропускаем строки без ->

        string leftPart = trim(line.substr(0, arrowPos)); // Часть до ->
        string rightPart = trim(line.substr(arrowPos + 2)); // Часть после ->

        // Разделяем правую часть по /
        size_t slashPos = rightPart.find('/');
        if (slashPos == string::npos) continue; // Пропускаем строки без /

        string beforeSlash = trim(rightPart.substr(0, slashPos)); // Часть между -> и /
        string afterSlash = trim(rightPart.substr(slashPos + 1)); // Часть после /

        // Заполняем ruleMap
        ruleMap.emplace_back(leftPart, beforeSlash);

        // Заполняем afterSlashMap
        vector<string> afterSlashParts;
        stringstream ss(afterSlash);
        string part;
        while (ss >> part) { // Разделяем по пробелам
            afterSlashParts.push_back(part);
        }
        afterSlashMap.emplace_back(leftPart, afterSlashParts);

        // Извлекаем нетерминалы
        vector<string> extracted = extractNonTerminals(leftPart);
        nonTerminals.insert(nonTerminals.end(), extracted.begin(), extracted.end());
    }

    file.close();
}

// Функция для заполнения вектора signs
void fillSigns(const vector<pair<string, string>>& ruleMap,
               const vector<pair<string, vector<string>>>& afterSlashMap,
               vector<Sign>& signs) {
    size_t currentIndex = 1; // Порядковый номер для знаков

    // Сначала добавляем ключи из ruleMap (левая часть)
    for (const auto& pair : ruleMap) {
        string key = pair.first;
        bool isTerminal = (key.front() != '<' || key.back() != '>'); // Определяем, терминал это или нетерминал
        string value = key; // Оставляем скобки для нетерминалов

        // Находим направляющие символы для текущего ключа
        vector<string> directionSymbols;
        for (const auto& afterSlashPair : afterSlashMap) {
            if (afterSlashPair.first == key) {
                directionSymbols = afterSlashPair.second;
                break;
            }
        }

        // Добавляем ключ в вектор signs (левая часть)
        signs.emplace_back(currentIndex++, value, directionSymbols, isTerminal, "left");
    }

    // Затем добавляем значения из ruleMap (правая часть)
    for (const auto& pair : ruleMap) {
        string value = pair.second;
        vector<string> tokens = splitIntoTerminalsAndNonTerminals(value);

        // Создаем временный вектор для хранения пар (index, value) из правой части
        vector<::pair<size_t, string>> rightPartPairs;

        for (const string& token : tokens) {
            bool isTerminal = (token.front() != '<' || token.back() != '>'); // Определяем, терминал это или нетерминал
            string tokenValue = token; // Оставляем скобки для нетерминалов

            // Находим направляющие символы для текущего ключа
            vector<string> directionSymbols;
            if (isTerminal) {
                directionSymbols.push_back(tokenValue);
            } else {
                for (const auto& afterSlashPair : afterSlashMap) {
                    if (afterSlashPair.first == pair.first) {
                        directionSymbols = afterSlashPair.second;
                        break;
                    }
                }
            }

            // Добавляем значение в вектор signs (правая часть)
            signs.emplace_back(currentIndex++, tokenValue, directionSymbols, isTerminal, "right");

            // Сохраняем пару (index, value) для правой части
            rightPartPairs.emplace_back(signs.back().index, signs.back().value);
        }

        // Находим соответствующий нетерминал из левой части и добавляем rightPartPairs
        for (auto& sign : signs) {
            if (sign.part == "left" && sign.value == pair.first) {
                // Добавляем rightPartPairs к существующим leftPartPairs
                sign.leftPartPairs.insert(sign.leftPartPairs.end(), rightPartPairs.begin(), rightPartPairs.end());
            }
        }
    }
}

// Функция для вывода результатов
void printResults(const vector<pair<string, string>>& ruleMap,
                  const vector<pair<string, vector<string>>>& afterSlashMap,
                  const vector<string>& nonTerminals,
                  const vector<Sign>& signs) {
    cout << "Rule Map:" << endl;
    for (const auto& pair : ruleMap) {
        cout << pair.first << " -> " << pair.second << endl;
    }

    cout << "\nAfter Slash Map:" << endl;
    for (const auto& pair : afterSlashMap) {
        cout << pair.first << " -> ";
        for (const string& part : pair.second) {
            cout << part << " ";
        }
        cout << endl;
    }

    cout << "\nNonTerminals:" << endl;
    for (const string& nt : nonTerminals) {
        cout << nt << " ";
    }
    cout << endl;

    cout << "\nSigns:" << endl;
    for (const Sign& sign : signs) {
        cout << sign << endl;
    }
}

int main() {
    vector<pair<string, string>> ruleMap; // Ключ: слово до ->, значение: строка между -> и /
    vector<pair<string, vector<string>>> afterSlashMap; // Ключ: слово до ->, значение: строка после /, разделенная пробелами
    vector<string> nonTerminals; // Массив слов между <>
    vector<Sign> signs; // Вектор для хранения знаков

    // Чтение файла и заполнение ruleMap, afterSlashMap и nonTerminals
    readGrammarFile("grammar.txt", ruleMap, afterSlashMap, nonTerminals);

    // Заполнение вектора signs
    fillSigns(ruleMap, afterSlashMap, signs);

    // Вывод результатов
    printResults(ruleMap, afterSlashMap, nonTerminals, signs);

    return 0;
}