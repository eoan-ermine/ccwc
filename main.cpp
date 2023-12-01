#include <iostream>
#include <string_view>
#include <fstream>
#include <filesystem>
#include <array>
#include <cstring>

using namespace std::literals;
namespace fs = std::filesystem;

int main(int argc, char* argv[]) {
    if (argc != 3) {
        return -1;
    }

    fs::path filename = argv[2];
    fs::path absolutePath = fs::current_path() / filename;

    if (argv[1] == "-c"sv) {
        std::error_code ec;
        if (const auto size = fs::file_size(absolutePath, ec); ec) {
            std::cout << absolutePath << ": " << ec.message() << '\n';
        } else {
            std::cout << size << ' ' << argv[2] << '\n';
        }
    } else if (argv[1] == "-l"sv) {
        constexpr auto BUFFER_SIZE = 16*1024;
        std::array<char, BUFFER_SIZE> buffer;
        std::ifstream file{argv[2]};

        uintmax_t lines = 0;
        while (size_t nBytes = (file.read(buffer.data(), BUFFER_SIZE), file.gcount())) {
            if (nBytes == 0) break;

            for(char *p = buffer.data(); (p = (char*) memchr(p, '\n', (buffer.data() + nBytes) - p)); ++p)
                ++lines;
        }

        std::cout << lines << ' ' << argv[2] << '\n';
    }

    return 0;
}
