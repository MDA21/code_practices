#pragma once

#include <cstddef>
#include <cstdint>
#include <span>

// Row-major matrix. Caller guarantees values.size() == rows * columns.
// Dimensions and the exact sum fit in size_t and uint64_t respectively.
// Either zero dimension means an empty matrix: return 0.
// Do not modify the input or allocate another matrix.

// Visit one complete row at a time, from left to right, top to bottom.
std::uint64_t sumRows(std::span<const std::uint32_t> values,
                      std::size_t rows, std::size_t columns);

// Visit one complete column at a time, from top to bottom, left to right.
std::uint64_t sumColumns(std::span<const std::uint32_t> values,
                         std::size_t rows, std::size_t columns);

void testMatrixTraversal();
void benchmarkMatrixTraversal();
