#include "MatrixTraversal.h"

#include <stdexcept>

std::uint64_t sumRows(std::span<const std::uint32_t> values,
                      std::size_t rows, std::size_t columns) {
    // TODO: implement the specified traversal. Remove these placeholder lines.
    std::uint64_t res=0;
    for (std::size_t i = 0;i < rows;++i) {
        for (std::size_t j = 0;j < columns;++j) {
            res += values[j+i*columns];
        }
    }
    return res;
}

std::uint64_t sumColumns(std::span<const std::uint32_t> values,
                         std::size_t rows, std::size_t columns) {
    // TODO: implement the specified traversal. Remove these placeholder lines.
    std::uint64_t res = 0;
    for (std::size_t j = 0; j < columns;++j) {
        for (std::size_t i = 0;i < rows;++i) {
            res += values[j + i * columns];
        }
    }
    return res;
}
