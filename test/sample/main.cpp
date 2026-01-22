#include <catch2/catch_test_macros.hpp>

#include <filesystem>
#include <fstream>

#include "libccwc/for_each_character.hpp"
#include "libcount/count.hpp"

TEST_CASE("test.txt", "[ccwc]") {
    {
        using namespace eoanermine::ccwc;
        using namespace eoanermine;

        std::ifstream file{
            std::filesystem::canonical("/proc/self/exe").parent_path() /
            "test.txt"};
        std::istream *stream = &file;

        std::setlocale(LC_ALL, "");
        std::mblen(nullptr, 0); // reset the conversion state

        auto results = forEachCharacter<BufferType>(
            *stream, {count::bytes<BufferType>, count::lines<BufferType>,
                      count::words<BufferType>, count::characters<BufferType>});

        REQUIRE(results[0] == 342190);
        REQUIRE(results[1] == 7145);
        REQUIRE(results[2] == 58164);
        REQUIRE(results[3] == 339292);
    }
}
