#include <cstring>
#include <fstream>
#include <functional>
#include <iostream>
#include <vector>

#include <boost/program_options.hpp>

#include "libccwc/for_each_character.hpp"
#include "libcount/count.hpp"

using namespace std::literals;
using namespace eoanermine::ccwc;
using namespace eoanermine;

static void resetMultibyteState() {
    std::setlocale(LC_ALL, "");
    std::mblen(nullptr, 0);
}

static auto getMultibyteCountFunction() {
    return MB_CUR_MAX > 1 ? count::characters<BufferType>
                          : count::bytes<BufferType>;
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

    if ((argc == 2 && std::strlen(argv[1]) > 0 && argv[1][0] == '-') || argc >= 3) {
        resetMultibyteState();
        
        // Collect all required count functions
        std::vector<std::function<size_t(const BufferType&, size_t)>> functions;
        
        if (vm.count("lines")) {
            functions.push_back(count::lines<BufferType>);
        }
        
        if (vm.count("words")) {
            functions.push_back(count::words<BufferType>);
        }
        
        if (vm.count("chars")) {
            functions.push_back(getMultibyteCountFunction());
        }
        
        if (vm.count("bytes")) {
            functions.push_back(count::bytes<BufferType>);
        }
        
        // Read stream once and compute all counts
        BufferType buffer;
        std::vector<size_t> results(functions.size(), 0);
        while (size_t nBytes = (stream->read(buffer.data(), BUFFER_SIZE), stream->gcount())) {
            for (size_t i = 0; i < functions.size(); ++i) {
                results[i] += functions[i](buffer, nBytes);
            }
        }
        
        // Output results
        for (size_t result : results) {
            std::cout << result << ' ';
        }
        if (input_file.size()) {
            std::cout << input_file;
        }
        std::cout << '\n';
    } else if (argc <= 2) {
        resetMultibyteState();
        auto [lines, words, chars, bytes] = forEachCharacter<BufferType>(
            *stream, count::lines<BufferType>, count::words<BufferType>,
            getMultibyteCountFunction(), count::bytes<BufferType>);
        std::cout << lines << ' ' << words << ' ' << bytes
                  << (input_file.size() ? " " : "") << input_file << '\n';
    }

    return 0;
}
