#include <iostream>
#include <string_view>
#include <fstream>
#include <filesystem>
#include <array>
#include <cstring>
#include <fmt/format.h>

using namespace std::literals;
namespace fs = std::filesystem;

int main(int argc, char* argv[]) {
    auto resetStream = [](std::istream &stream) {
        stream.clear();                 // clear fail and eof bits
        stream.seekg(0, std::ios::beg); // back to the start!
    };
    auto forEachCharacter = [](std::istream &stream, auto F) {
        constexpr auto BUFFER_SIZE = 16*1024;
        std::array<char, BUFFER_SIZE> buffer;

        uintmax_t accumulator = 0;
        while (size_t nBytes = (stream.read(buffer.data(), BUFFER_SIZE), stream.gcount())) {
            if (nBytes == 0) break;

            F(accumulator, buffer, nBytes);
        }

        return accumulator;
    };
    auto bytesCount = [](auto &accumulator, auto &buffer, auto nBytes) {
        accumulator += nBytes;
    };
    auto linesCount = [](auto &accumulator, auto &buffer, auto nBytes) {
        for(char *p = buffer.data(); (p = (char*) memchr(p, '\n', (buffer.data() + nBytes) - p)); ++p)
            ++accumulator;
    };
    auto wordsCount = [](auto &accumulator, auto &buffer, auto nBytes) {
        bool isSpace = false;
        for (auto it = buffer.data(), endIt = it + nBytes; it != endIt; ++it) {
            switch (*it) {
                case '\r':
                case '\n':
                case '\t':
                case '\f':
                case '\v':
                case ' ': {
                    if (!isSpace) {
                        ++accumulator;
                    }
                    isSpace = true;
                    break;
                }
                default: {
                    isSpace = false;
                }
            }
        }
    };
    auto multibyteCount = [](auto &accumulator, auto &buffer, auto nBytes) {
        for (auto it = buffer.data(), endIt = it + nBytes; it != endIt; ++accumulator) {
            const int next = std::mblen(it, endIt - it);
            if (next == -1)
                throw std::runtime_error("strlen_mb(): conversion error");
            it += next;
        }
    };

    if ((argc == 2 && argv[1][0] == '-') || argc == 3) {
        const char* filename = (argc == 3 ? argv[2] : "stdin");
        std::istream *stream = &std::cin;
        std::ifstream file;
        if (argc == 3) {
            file = std::ifstream{argv[2]};
            stream = &file;
        }

        uintmax_t accumulator = 0;
        if (argv[1] == "-c"sv) {
            accumulator = forEachCharacter(*stream, bytesCount);
        } else if (argv[1] == "-l"sv) {
            accumulator = forEachCharacter(*stream, linesCount);
        } else if (argv[1] == "-w"sv) {
            accumulator = forEachCharacter(*stream, wordsCount);
        } else if (argv[1] == "-m"sv) {
            std::setlocale(LC_ALL, "");
            std::mblen(nullptr, 0); // reset the conversion state

            if (MB_CUR_MAX > 1)
                accumulator = forEachCharacter(*stream, multibyteCount);
            else
                accumulator = forEachCharacter(*stream, bytesCount);
        }

        if (filename == "stdin"sv) {
            fmt::println("{}", accumulator);
        } else {
            fmt::println("{} {}", accumulator, filename);
        }
    } else if (argc <= 2) {
        const char* filename = (argc == 2 ? argv[1] : "stdin");
        std::istream *stream = &std::cin;
        std::ifstream file;
        if (argc == 2) {
            file = std::ifstream{argv[1]};
            stream = &file;
        }

        auto bytes = forEachCharacter(*stream, bytesCount);
        auto lines = (resetStream(*stream), forEachCharacter(*stream, linesCount));
        auto words = (resetStream(*stream), forEachCharacter(*stream, wordsCount));

        if (filename == "stdin"sv) {
            fmt::println("{} {} {}", lines, words, bytes);
        } else {
            fmt::println("{} {} {} {}", lines, words, bytes, filename);
        }
    }

    return 0;
}