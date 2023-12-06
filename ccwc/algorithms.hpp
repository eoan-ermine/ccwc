#pragma once

#include <array>
#include <cstdint>
#include <cstring>
#include <istream>

constexpr size_t BUFFER_SIZE = 16 * 1024;
using AccumulatorType = uintmax_t;
using BufferType = std::array<char, BUFFER_SIZE>;

template <typename... F>
constexpr auto forEachCharacter(std::istream &stream, F... fun) {
    return []<auto... Idx>(std::istream &stream, std::index_sequence<Idx...>,
                           F... fun) {
        BufferType buffer;
        std::array<AccumulatorType, sizeof...(F)> accumulators;
        while (size_t nBytes =
                   (stream.read(buffer.data(), BUFFER_SIZE), stream.gcount())) {
            if (nBytes == 0)
                break;
            (fun(accumulators[Idx], buffer, nBytes), ...);
        }
        return accumulators;
    }(stream, std::make_index_sequence<sizeof...(F)>(), fun...);
};

constexpr auto bytesCount(AccumulatorType &accumulator, BufferType &buffer,
                          size_t nBytes) {
    accumulator += nBytes;
}

constexpr auto linesCount(AccumulatorType &accumulator, BufferType &buffer,
                          size_t nBytes) {
    for (char *p = buffer.data();
         (p = (char *)memchr(p, '\n', (buffer.data() + nBytes) - p)); ++p)
        ++accumulator;
};

constexpr auto wordsCount(AccumulatorType &accumulator, BufferType &buffer,
                          size_t nBytes) {
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

constexpr auto multibyteCount(AccumulatorType &accumulator, BufferType &buffer,
                              size_t nBytes) {
    for (auto it = buffer.data(), endIt = it + nBytes; it != endIt;
         ++accumulator) {
        const int next = std::mblen(it, endIt - it);
        if (next == -1)
            throw std::runtime_error("strlen_mb(): conversion error");
        it += next;
    }
};