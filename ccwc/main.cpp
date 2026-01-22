#include <fstream>
#include <iostream>

#include <boost/program_options.hpp>

#include "countFunctions.hpp"

using namespace std::literals;

template <typename F>
static std::string handleArgument(std::istream *&stream, const F &function) {
    std::string result = std::to_string(forEachCharacter(*stream, function)[0]);
    result += " ";
    stream->clear();
    stream->seekg(0);
    return result;
}

static void resetMultibyteState() {
    std::setlocale(LC_ALL, "");
    std::mblen(nullptr, 0);
}

static auto getMultibyteCountFunction() {
    return MB_CUR_MAX > 1 ? multibyteCount : bytesCount;
}

int main(int argc, char *argv[]) {
    namespace po = boost::program_options;

    std::string input_file{};
    po::options_description desc{"Options"};
    desc.add_options()("help,h", "produce help message")(
        "bytes,c", "print count of bytes")("chars,m", "print count of symbols")(
        "lines,l", "print count of lines")("words,w", "print count of words")(
        "input-file", po::value<std::string>(&input_file));

    po::positional_options_description pos_desc;
    pos_desc.add("input-file", -1);

    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv)
                  .options(desc)
                  .positional(pos_desc)
                  .run(),
              vm);
    po::notify(vm);

    if (vm.count("help")) {
        std::cout << desc << "\n";
        return 0;
    }

    std::istream *stream = &std::cin;
    std::ifstream file;
    if (input_file.size()) {
        file = std::ifstream{input_file};
        if (!file) {
            std::cerr << "Cannot open input file: " << input_file << "\n";
            return 1;
        }
        stream = &file;
    }

    if ((argc == 2 && argv[1][0] == '-') || argc >= 3) {
        std::string result_string;

        if (vm.count("lines")) {
            result_string += handleArgument(stream, linesCount);
        }

        if (vm.count("words")) {
            result_string += handleArgument(stream, wordsCount);
        }

        if (vm.count("chars")) {
            resetMultibyteState();
            result_string +=
                handleArgument(stream, getMultibyteCountFunction());
        }

        if (vm.count("bytes")) {
            result_string += handleArgument(stream, bytesCount);
        }

        std::cout << result_string << ' ' << input_file << '\n';
    } else if (argc <= 2) {
        resetMultibyteState();
        auto [lines, words, chars, bytes] =
            forEachCharacter(*stream, linesCount, wordsCount,
                             getMultibyteCountFunction(), bytesCount);
        std::cout << lines << ' ' << words << ' ' << bytes
                  << (input_file.size() ? " " : "") << input_file << '\n';
    }

    return 0;
}
