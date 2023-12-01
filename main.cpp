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
    if (argc == 1) {
        return -1;
    }

    auto forEachCharacter = [](const char* filename, auto F) {
        constexpr auto BUFFER_SIZE = 16*1024;
        std::array<char, BUFFER_SIZE> buffer;
        std::ifstream file{filename};

        uintmax_t accumulator = 0;
        while (size_t nBytes = (file.read(buffer.data(), BUFFER_SIZE), file.gcount())) {
            if (nBytes == 0) break;

            F(accumulator, buffer, nBytes);
        }

        return accumulator;
    };
    auto bytesCount = [](const char* filename) -> uintmax_t  {
        fs::path absolutePath = fs::current_path() / filename;
        return fs::file_size(absolutePath);
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


    if (argc == 2) {
        const char* filename = argv[1];

        auto bytes = bytesCount(filename);
        auto lines = forEachCharacter(filename, linesCount);
        auto words = forEachCharacter(filename, wordsCount);
        fmt::println("{}\t{}\t{} {}", words, lines, bytes, filename);
    } else if (argc == 3) {
        const char* filename = argv[2];

        uintmax_t accumulator;
        if (argv[1] == "-c"sv) {
            accumulator = bytesCount(filename);
        } else if (argv[1] == "-l"sv) {
            accumulator = forEachCharacter(filename, linesCount);
        } else if (argv[1] == "-w"sv) {
            accumulator = forEachCharacter(filename, wordsCount);
        } else if (argv[1] == "-m"sv) {
            std::setlocale(LC_ALL, "");
            std::mblen(nullptr, 0); // reset the conversion state

            if (MB_CUR_MAX > 1)
                accumulator = forEachCharacter(filename, multibyteCount);
            else
                accumulator = bytesCount(filename);
        }
        fmt::println("{} {}", accumulator, filename);
    }

    return 0;
}
