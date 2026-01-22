#include <array>
#include <cstddef>
#include <istream>

namespace eoanermine {

namespace ccwc {

constexpr size_t BUFFER_SIZE = 16 * 1024;
using BufferType = std::array<char, BUFFER_SIZE>;

template <typename BufferType, typename... F>
constexpr auto forEachCharacter(std::istream &stream, const F &...fun) {
    return [&]<auto... Idx>(std::index_sequence<Idx...>) {
        BufferType buffer;
        std::array<std::size_t, sizeof...(F)> accumulators = {};
        while (size_t nBytes =
                   (stream.read(buffer.data(), BUFFER_SIZE), stream.gcount())) {
            if (nBytes == 0)
                break;
            ((accumulators[Idx] += fun(buffer, nBytes)), ...);
        }
        return accumulators;
    }(std::make_index_sequence<sizeof...(F)>());
}

} // namespace ccwc

} // namespace eoanermine