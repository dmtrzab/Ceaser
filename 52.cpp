#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <algorithm>
#include <filesystem>  

namespace fs = std::filesystem;

std::map<char, double> calculateFrequencies(const std::string& text) {
    std::map<char, double> frequencies;
    int totalLetters = 0;

    for (char ch : text) {
        if (isalpha(ch)) {
            char lowerCh = tolower(ch);
            frequencies[lowerCh]++;
            totalLetters++;
        }
    }

    for (auto& entry : frequencies) {
        entry.second = (entry.second / totalLetters) * 100;
    }

    return frequencies;
}

std::string readFile(const std::string& fileName) {
    std::ifstream file(fileName);
    if (!file.is_open()) {
        std::cerr << "Не удалось открыть файл: " << fileName << std::endl;
        exit(1);
    }

    std::string text((std::istreambuf_iterator<char>(file)),
                      std::istreambuf_iterator<char>());
    file.close();
    return text;
}

void writeFile(const std::string& fileName, const std::string& text) {
    std::ofstream file(fileName);
    if (!file.is_open()) {
        std::cerr << "Не удалось открыть файл для записи: " << fileName << std::endl;
        exit(1);
    }
    file << text;
    file.close();
}

std::string encrypt(const std::string& text, int shift) {
    std::string encryptedText = text;
    
    for (char& ch : encryptedText) {
        if (isalpha(ch)) {
            bool isUpper = isupper(ch);
            ch = tolower(ch);
            ch = (ch - 'a' + shift) % 26 + 'a';
            if (isUpper) {
                ch = toupper(ch);
            }
        }
    }

    return encryptedText;
}

std::string decrypt(const std::string& cipherText, int shift) {
    std::string decryptedText = cipherText;

    for (char& ch : decryptedText) {
        if (isalpha(ch)) {
            bool isUpper = isupper(ch);
            ch = tolower(ch);
            ch = (ch - 'a' - shift + 26) % 26 + 'a';
            if (isUpper) {
                ch = toupper(ch);
            }
        }
    }

    return decryptedText;
}

int findShift(const std::map<char, double>& baseFreq, const std::map<char, double>& cipherFreq) {
    char mostFrequentBaseChar = std::max_element(baseFreq.begin(), baseFreq.end(), [](const std::pair<char, double>& a, const std::pair<char, double>& b) {return a.second < b.second;})->first;

    char mostFrequentCipherChar = std::max_element(cipherFreq.begin(), cipherFreq.end(),[](const std::pair<char, double>& a, const std::pair<char, double>& b) {return a.second < b.second;})->first;

    int shift = (mostFrequentCipherChar - mostFrequentBaseChar + 26) % 26;
    return shift;
}

bool isDirectory(const std::string& path) {
    return fs::is_directory(path);
}

int main() {
    int mode;
    std::cout << "Выберите режим:\n1. Шифрование\n2. Дешифрование\n";
    std::cin >> mode;

    if (mode == 1) {
        std::string inputFile, outputFile;
        int shift;
        std::cout << "Введите путь к файлу для шифрования: ";
        std::cin >> inputFile;
        std::cout << "Введите путь для сохранения зашифрованного файла: ";
        std::cin >> outputFile;
        std::cout << "Введите сдвиг для шифра Цезаря: ";
        std::cin >> shift;

        std::string plainText = readFile(inputFile);
        std::string encryptedText = encrypt(plainText, shift);
        writeFile(outputFile, encryptedText);

        std::cout << "Текст успешно зашифрован и сохранен в " << outputFile << std::endl;

    } else if (mode == 2) {
        // Дешифрование
        std::string baseFile, cipherFile, outputPath;
        std::cout << "Введите путь к базовому файлу (например, текст книги): ";
        std::cin >> baseFile;
        std::cout << "Введите путь к зашифрованному файлу: ";
        std::cin >> cipherFile;
        std::cout << "Введите путь для сохранения дешифрованного файла (папка или файл): ";
        std::cin >> outputPath;

        std::string baseText = readFile(baseFile);
        std::map<char, double> baseFrequencies = calculateFrequencies(baseText);

        std::string cipherText = readFile(cipherFile);
        std::map<char, double> cipherFrequencies = calculateFrequencies(cipherText);

        int shift = findShift(baseFrequencies, cipherFrequencies);
        std::cout << "Предполагаемый сдвиг: " << shift << std::endl;

        std::string decryptedText = decrypt(cipherText, shift);

        if (isDirectory(outputPath)) {
            std::string outputFile = outputPath + "/decryptedText.txt";
            writeFile(outputFile, decryptedText);
            std::cout << "Дешифрованный текст сохранен в файл: " << outputFile << std::endl;
        } else {
            writeFile(outputPath, decryptedText);
            std::cout << "Дешифрованный текст перезаписан в файл: " << outputPath << std::endl;
        }

    } else {
        std::cerr << "Неверный выбор режима!" << std::endl;
    }

    return 0;
}
