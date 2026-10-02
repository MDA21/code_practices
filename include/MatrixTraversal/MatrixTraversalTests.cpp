#include "MatrixTraversal.h"

#include <algorithm>
#include <array>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>

namespace {
    using Kernel = std::uint64_t (*)(std::span<const std::uint32_t>,
                                    std::size_t, std::size_t);

    void check(std::span<const std::uint32_t> values, std::size_t rows,
               std::size_t columns, std::uint64_t expected) {
        // Runtime checks remain enabled in Release as well as Debug.
        if (sumRows(values, rows, columns) != expected) {
            throw std::runtime_error("sumRows: incorrect sum");
        }
        if (sumColumns(values, rows, columns) != expected) {
            throw std::runtime_error("sumColumns: incorrect sum");
        }
    }

#ifdef NDEBUG
    double measure(Kernel candidate, std::span<const std::uint32_t> values,
                   std::size_t rows, std::size_t columns, std::uint64_t expected) {
        // A volatile function pointer keeps repeated calls observable to the compiler.
        Kernel volatile kernel = candidate;
        const auto start = std::chrono::steady_clock::now();
        const auto result = kernel(values, rows, columns);
        const auto end = std::chrono::steady_clock::now();
        if (result != expected) {
            throw std::runtime_error("Benchmark correctness check failed");
        }
        return std::chrono::duration<double, std::milli>(end - start).count();
    }
#endif
}

void testMatrixTraversal() {
    check({}, 0, 0, 0);
    check({}, 0, 7, 0);
    check({}, 5, 0, 0);
    const std::array<std::uint32_t, 1> single{42};
    check(single, 1, 1, 42);
    const std::array<std::uint32_t, 6> rectangle{1, 2, 3, 4, 5, 6};
    check(rectangle, 2, 3, 21);
    check(rectangle, 3, 2, 21);
    check(rectangle, 1, 6, 21);
    check(rectangle, 6, 1, 21);
    const std::array<std::uint32_t, 6> zeros{};
    check(zeros, 2, 3, 0);
    constexpr auto largest = std::numeric_limits<std::uint32_t>::max();
    const std::array<std::uint32_t, 2> wideSum{largest, largest};
    check(wideSum, 1, 2, 8589934590ULL);
    std::cout << "All MatrixTraversal correctness tests passed.\n";
}

void benchmarkMatrixTraversal() {
#ifndef NDEBUG
    throw std::runtime_error("Use a Release build for the benchmark");
#else
    constexpr std::size_t repeats = 5;
    std::cout << "Median of 5 samples; allocation and initialization excluded.\n"
              << "rows,columns,MiB,row_ms,column_ms,column_over_row\n";
    for (const auto [rows, columns] :
         {std::pair<std::size_t, std::size_t>{64, 64}, {1024, 1024},
          {4096, 4096}, {4096, 4095}}) {
        std::vector<std::uint32_t> values(rows * columns);
        std::uint64_t expected = 0;
        // Generate input and its checksum outside the timed region.
        for (std::size_t i = 0; i < values.size(); ++i) {
            values[i] = static_cast<std::uint32_t>((i * 17 + 13) % 251);
            expected += values[i];
        }
        (void)measure(sumRows, values, rows, columns, expected);
        (void)measure(sumColumns, values, rows, columns, expected);
        std::array<double, repeats> rowTimes{}, columnTimes{};
        for (std::size_t sample = 0; sample < repeats; ++sample) {
            // Alternate order to reduce systematic first/second-run bias.
            if (sample % 2 == 0) {
                rowTimes[sample] = measure(sumRows, values, rows, columns, expected);
                columnTimes[sample] = measure(sumColumns, values, rows, columns, expected);
            }
            else {
                columnTimes[sample] = measure(sumColumns, values, rows, columns, expected);
                rowTimes[sample] = measure(sumRows, values, rows, columns, expected);
            }
        }
        std::sort(rowTimes.begin(), rowTimes.end());
        std::sort(columnTimes.begin(), columnTimes.end());
        const double rowMs = rowTimes[repeats / 2];
        const double columnMs = columnTimes[repeats / 2];
        const double mib = static_cast<double>(values.size() * sizeof(values[0])) / (1024 * 1024);
        std::cout << rows << ',' << columns << ',' << std::fixed << std::setprecision(4)
                  << mib << ',' << rowMs << ',' << columnMs << ',';
        if (rowMs > 0) std::cout << columnMs / rowMs;
        else std::cout << "n/a";
        std::cout << '\n';
    }
#endif
}
