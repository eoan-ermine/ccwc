#include <iostream>
#include <string_view>
#include <fstream>
#include <filesystem>

using namespace std::literals;
namespace fs = std::filesystem;

int main(int argc, char* argv[]) {
    if (argc != 3) {
        return -1;
    }
    if (argv[1] != "-c"sv) {
        return -1;
    }

    fs::path filename = argv[2];
    fs::path absolutePath = fs::current_path() / filename;

    std::error_code ec;
    if (const auto size = fs::file_size(absolutePath, ec); ec) {
        std::cout << absolutePath << ": " << ec.message() << '\n';
    } else {
        std::cout << size << ' ' << argv[2] << '\n';
    }

    return 0;
}
