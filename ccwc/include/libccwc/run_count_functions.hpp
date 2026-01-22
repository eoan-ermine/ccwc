#include <array>
#include <cstddef>
#include <istream>
#include <vector>
#include <functional>

namespace eoanermine {

namespace ccwc {

constexpr size_t BUFFER_SIZE = 16 * 1024;
using BufferType = std::array<char, BUFFER_SIZE>;

template <typename BufferType>
auto runCountFunctions(std::istream &stream, std::vector<std::function<size_t(const BufferType&, size_t)>> functions) {
    BufferType buffer;
    std::vector<std::size_t> accumulators(functions.size(), 0);

    while (size_t nBytes =
               (stream.read(buffer.data(), BUFFER_SIZE), stream.gcount())) {
        if (nBytes == 0)
            break;
        for (std::size_t i = 0; i != functions.size(); ++i) {
            accumulators[i] += functions[i](buffer, nBytes);
        }
    }
    return accumulators;
}

} // namespace ccwc

} // namespace eoanermine