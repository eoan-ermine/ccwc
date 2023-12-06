#include <fstream>
#include <iostream>

#include "algorithms.hpp"

using namespace std::literals;

int main(int argc, char *argv[]) {
    std::istream *stream = &std::cin;
    std::ifstream file;

    if ((argc == 2 && argv[1][0] == '-') || argc == 3) {
        const char *path = (argc == 3 ? argv[2] : nullptr);
        if (argc == 3)
            stream = &(file = std::ifstream{argv[2]});

        uintmax_t accumulator = 0;
        if (argv[1] == "-c"sv) {
            accumulator = forEachCharacter(*stream, bytesCount)[0];
        } else if (argv[1] == "-l"sv) {
            accumulator = forEachCharacter(*stream, linesCount)[0];
        } else if (argv[1] == "-w"sv) {
            accumulator = forEachCharacter(*stream, wordsCount)[0];
        } else if (argv[1] == "-m"sv) {
            std::setlocale(LC_ALL, "");
            std::mblen(nullptr, 0); // reset the conversion state

            if (MB_CUR_MAX > 1)
                accumulator = forEachCharacter(*stream, multibyteCount)[0];
            else
                accumulator = forEachCharacter(*stream, bytesCount)[0];
        }

        std::cout << accumulator << ' ' << path << (path ? " " : "") << (path ? path : "") << '\n';
    } else if (argc <= 2) {
        const char *path = (argc == 2 ? argv[1] : nullptr);
        if (argc == 2)
            stream = &(file = std::ifstream{argv[1]});

        auto [bytes, lines, words] = forEachCharacter(*stream, bytesCount, linesCount, wordsCount);
        std::cout << lines << ' ' << words << ' ' << bytes << (path ? " " : "") << (path ? path : "") << '\n';
    }

    return 0;
}