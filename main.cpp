#include <iostream>
#include <string_view>
#include <fstream>
#include <filesystem>
#include <array>
#include <cstring>
#include <fmt/format.h>
#include <expected>

using namespace std::literals;
namespace fs = std::filesystem;

int main(int argc, char* argv[]) {
    if (argc != 3) {
        return -1;
    }

    const char* filename = argv[2];

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

    auto bytesCount = [](const char* filename) -> std::expected<long int, std::error_code> {
        fs::path absolutePath = fs::current_path() / filename;
        std::error_code ec;
        if (const auto size = fs::file_size(absolutePath, ec); ec) {
            return std::unexpected(ec);
        } else {
            return size;
        }
    };
    auto linesCount = [](auto &accumulator, auto &buffer, auto nBytes) {
        for(char *p = buffer.data(); (p = (char*) memchr(p, '\n', (buffer.data() + nBytes) - p)); ++p)
            ++accumulator;
    };
    auto wordsCount = [](auto &accumulator, auto &buffer, auto nBytes) {
        buffer[nBytes] = ' ';
        bool isSpace = false;
        for (auto it = buffer.data(), endIt = it + nBytes + 1; it != endIt; ++it) {
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
    auto multibytesCount = [](auto &accumulator, auto &buffer, auto nBytes) {
        for (auto it = buffer.data(), endIt = it + nBytes; it != endIt; ++accumulator) {
            const int next = std::mblen(it, endIt - it);
            if (next == -1)
                throw std::runtime_error("strlen_mb(): conversion error");
            it += next;
        }
    };


    if (argv[1] == "-c"sv) {
        auto res = bytesCount(filename);
        if (res.has_value()) {
            fmt::println("{} {}", filename, res.value());
        } else {
            fmt::println("{}: {}", filename, res.error().message());
        }
    } else if (argv[1] == "-l"sv) {
        auto accumulator = forEachCharacter(filename, linesCount);
        fmt::println("{} {}", accumulator, filename);
    } else if (argv[1] == "-w"sv) {
        auto accumulator = forEachCharacter(filename, wordsCount);
        fmt::println("{} {}", accumulator, filename);
    } else if (argv[1] == "-m"sv) {
        std::setlocale(LC_ALL, "");
        std::mblen(nullptr, 0); // reset the conversion state

        uintmax_t accumulator;
        if (MB_CUR_MAX > 1) {
            accumulator = forEachCharacter(filename, multibytesCount);
            fmt::println("{} {}", accumulator, filename);
        } else {
            auto res = bytesCount(filename);
            if (res.has_value()) {
                fmt::println("{} {}", filename, res.value());
            } else {
                fmt::println("{}: {}", filename, res.error().message());
            }
        }
    }

    return 0;
}
