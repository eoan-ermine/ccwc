#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <stdexcept>

namespace eoanermine {

namespace count {

template <typename BufferType>
constexpr size_t bytes([[maybe_unused]] const BufferType &_, size_t nBytes) {
    return nBytes;
}

template <typename BufferType>
inline size_t lines(const BufferType &buffer, size_t nBytes) {
    size_t linesCount{0};
    for (const char *p = buffer.data();
         (p = static_cast<const char*>(std::memchr(p, '\n', (buffer.data() + nBytes) - p))); ++p)
        ++linesCount;
    return linesCount;
}

template <typename BufferType>
inline size_t words(const BufferType &buffer, size_t nBytes) {
    size_t wordsCount{0};
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

template <typename BufferType>
inline size_t characters(const BufferType &buffer, size_t nBytes) {
    size_t multibyteCount{0};
    for (auto it = buffer.data(), endIt = it + nBytes; it != endIt;
         ++multibyteCount) {
        const int next = std::mblen(it, endIt - it);
        if (next == -1)
            throw std::runtime_error("strlen_mb(): conversion error");
        if (next == 0)
            break;  // Null character encountered
        it += next;
    }
    return multibyteCount;
}

} // namespace count

} // namespace eoanermine