#include "MatrixTranspose.h"

#include <stdexcept>

void transposeNaive(std::span<const std::uint32_t> src,
                    std::span<std::uint32_t> dst,
                    std::size_t rows, std::size_t columns) {
    // TODO: implement the baseline traversal.
    (void)src;
    (void)dst;
    (void)rows;
    (void)columns;
    throw std::logic_error("TODO: implement transposeNaive in MatrixTranspose.cpp");
}

void transposeBlocked(std::span<const std::uint32_t> src,
                      std::span<std::uint32_t> dst,
                      std::size_t rows, std::size_t columns,
                      std::size_t tileSize) {
    // TODO: implement and measure tiled traversal.
    (void)src;
    (void)dst;
    (void)rows;
    (void)columns;
    (void)tileSize;
    throw std::logic_error("TODO: implement transposeBlocked in MatrixTranspose.cpp");
}
