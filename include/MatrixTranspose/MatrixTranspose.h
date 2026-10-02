#pragma once

#include <cstddef>
#include <cstdint>
#include <span>

// Both matrices use row-major storage. Source shape: rows x columns.
// Destination shape: columns x rows. src and dst never overlap.
// Caller guarantees rows * columns fits size_t and equals both span sizes.
// Either zero dimension is an empty matrix; do not read or write elements.
// Every source element (r, c) becomes destination element (c, r).

// Baseline: visit source rows from top to bottom, each from left to right.
void transposeNaive(std::span<const std::uint32_t> src,
                    std::span<std::uint32_t> dst,
                    std::size_t rows, std::size_t columns);

// Process rectangular tiles with at most tileSize rows and tileSize columns.
// Complete one tile before starting the next. Edge tiles may be smaller.
// Do not allocate temporary arrays or change src. Use O(1) extra space.
// tileSize == 0: throw std::invalid_argument before any write, even if empty.
void transposeBlocked(std::span<const std::uint32_t> src,
                      std::span<std::uint32_t> dst,
                      std::size_t rows, std::size_t columns,
                      std::size_t tileSize);

void testMatrixTranspose();
void benchmarkMatrixTranspose();
