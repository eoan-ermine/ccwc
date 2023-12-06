#include <catch2/catch_test_macros.hpp>

#include <fstream>

#include "../../ccwc/algorithms.hpp"

TEST_CASE("test.txt", "[ccwc]") {
    {
        std::ifstream file{"test.txt"};
        std::istream *stream = &file;

        std::setlocale(LC_ALL, "");
        std::mblen(nullptr, 0); // reset the conversion state

        auto [bytes, lines, words, multibytes] = forEachCharacter(
            *stream, bytesCount, linesCount, wordsCount, multibyteCount);

        REQUIRE(bytes == 342190);
        REQUIRE(lines == 7145);
        REQUIRE(words == 58164);
        REQUIRE(multibytes == 339292);
    }
}
