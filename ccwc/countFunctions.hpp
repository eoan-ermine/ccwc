#pragma once

#include <array>
#include <cstdint>
#include <cstring>
#include <istream>

constexpr size_t BUFFER_SIZE = 16 * 1024;
using AccumulatorType = uintmax_t;
using BufferType = std::array<char, BUFFER_SIZE>;

template <typename... F>
constexpr auto forEachCharacter(std::istream &stream, const F&... fun) {
    return [&]<auto... Idx>(std::index_sequence<Idx...>) {
        BufferType buffer;
        std::array<AccumulatorType, sizeof...(F)> accumulators = {};
        while (size_t nBytes =
                   (stream.read(buffer.data(), BUFFER_SIZE), stream.gcount())) {
            if (nBytes == 0)
                break;
            ((accumulators[Idx] += fun(buffer, nBytes)), ...);
        }
        return accumulators;
    }(std::make_index_sequence<sizeof...(F)>());
}

constexpr auto bytesCount([[maybe_unused]] const BufferType &_, size_t nBytes) {
    return nBytes;
}

constexpr auto linesCount(const BufferType &buffer, size_t nBytes) {
    AccumulatorType linesCount{0};
    for (const char *p = buffer.data();
         (p = (char *)memchr(p, '\n', (buffer.data() + nBytes) - p)); ++p)
        ++linesCount;
    return linesCount;
}

constexpr auto wordsCount(const BufferType &buffer, size_t nBytes) {
    AccumulatorType wordsCount{0};
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
                ++wordsCount;
            isSpace = true;
            break;
        }
        default: {
            isSpace = false;
        }
        }
    }
    return wordsCount;
}

constexpr auto multibyteCount(const BufferType &buffer, size_t nBytes) {
    AccumulatorType multibyteCount{0};
    for (auto it = buffer.data(), endIt = it + nBytes; it != endIt;
         ++multibyteCount) {
        const int next = std::mblen(it, endIt - it);
        if (next == -1)
            throw std::runtime_error("strlen_mb(): conversion error");
        it += next;
    }
    return multibyteCount;
}
