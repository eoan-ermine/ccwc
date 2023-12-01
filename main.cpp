#include <iostream>
#include <string_view>
#include <fstream>
#include <array>
#include <cstring>
#include <cstdint>

using namespace std::literals;

constexpr auto forEachCharacter = []<typename... F>(std::istream &stream, F... fun) {
    return []<auto... Idx>(std::istream &stream, std::index_sequence<Idx...>, F... fun) {
        constexpr auto BUFFER_SIZE = 16 * 1024;
        std::array<char, BUFFER_SIZE> buffer;
        std::array<uintmax_t, sizeof...(F)> accumulators;
        while (size_t nBytes = (stream.read(buffer.data(), BUFFER_SIZE), stream.gcount())) {
            if (nBytes == 0) break;
            (fun(accumulators[Idx], buffer, nBytes), ...);
        }

        return accumulators;
    }(stream, std::make_index_sequence<sizeof...(F)>(), fun...);
};

constexpr auto bytesCount = [](auto &accumulator, auto &buffer, auto nBytes) {
    accumulator += nBytes;
};

constexpr auto linesCount = [](auto &accumulator, auto &buffer, auto nBytes) {
    for (char *p = buffer.data(); (p = (char *) memchr(p, '\n', (buffer.data() + nBytes) - p)); ++p)
        ++accumulator;
};

constexpr auto wordsCount = [](auto &accumulator, auto &buffer, auto nBytes) {
    bool isSpace = false;
    for (auto it = buffer.data(), endIt = it + nBytes; it != endIt; ++it) {
        switch (*it) {
            case '\r':
            case '\n':
            case '\t':
            case '\f':
            case '\v':
            case ' ': {
                if (!isSpace)
                    ++accumulator;
                isSpace = true;
                break;
            }
            default: {
                isSpace = false;
            }
        }
    }
};

constexpr auto multibyteCount = [](auto &accumulator, auto &buffer, auto nBytes) {
    for (auto it = buffer.data(), endIt = it + nBytes; it != endIt; ++accumulator) {
        const int next = std::mblen(it, endIt - it);
        if (next == -1)
            throw std::runtime_error("strlen_mb(): conversion error");
        it += next;
    }
};

int main(int argc, char *argv[]) {
    std::istream *stream = &std::cin;
    std::ifstream file;

    if ((argc == 2 && argv[1][0] == '-') || argc == 3) {
        const char *filename = (argc == 3 ? argv[2] : "stdin");
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

        if (filename == "stdin"sv) {
            std::cout << accumulator << '\n';
        } else {
            std::cout << accumulator << ' ' << filename << '\n';
        }
    } else if (argc <= 2) {
        const char *filename = (argc == 2 ? argv[1] : "stdin");
        if (argc == 2)
            stream = &(file = std::ifstream{argv[1]});

        auto [bytes, lines, words] = forEachCharacter(*stream, bytesCount, linesCount, wordsCount);
        if (filename == "stdin"sv) {
            std::cout << lines << ' ' << words << ' ' << bytes << '\n';
        } else {
            std::cout << lines << ' ' << words << ' ' << bytes << ' ' << filename << '\n';
        }
    }

    return 0;
}