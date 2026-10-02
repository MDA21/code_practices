#include "MatrixTranspose.h"

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
    constexpr std::uint32_t sentinel = 0xdeadbeef;

    void check(std::span<const std::uint32_t> input,
               std::size_t rows, std::size_t columns,
               std::span<const std::uint32_t> expected) {
        std::vector<std::uint32_t> source(input.begin(), input.end());
        // Canaries surround the writable destination to catch edge writes.
        std::vector<std::uint32_t> storage(expected.size() + 2, sentinel);
        auto output = std::span<std::uint32_t>(storage).subspan(1, expected.size());
        const auto verify = [&] {
            if (!std::equal(output.begin(), output.end(), expected.begin(), expected.end()))
                throw std::runtime_error("Incorrect transposed values");
            if (storage.front() != sentinel || storage.back() != sentinel)
                throw std::runtime_error("Destination boundary overwritten");
            if (!std::equal(source.begin(), source.end(), input.begin(), input.end()))
                throw std::runtime_error("Source was modified");
        };
        transposeNaive(source, output, rows, columns);
        verify();
        constexpr std::array<std::size_t, 6> tileSizes{
            1, 2, 3, 4, 32, std::numeric_limits<std::size_t>::max()};
        for (const auto tile : tileSizes) {
            std::fill(storage.begin(), storage.end(), sentinel);
            transposeBlocked(source, output, rows, columns, tile);
            verify();
        }
    }

#ifdef NDEBUG
    using Kernel = void (*)(std::span<const std::uint32_t>,
                            std::span<std::uint32_t>, std::size_t,
                            std::size_t, std::size_t);

    void baseline(std::span<const std::uint32_t> src,
                  std::span<std::uint32_t> dst,
                  std::size_t rows, std::size_t columns, std::size_t) {
        transposeNaive(src, dst, rows, columns);
    }

    double measure(Kernel candidate, std::span<const std::uint32_t> src,
                   std::span<std::uint32_t> dst, std::size_t rows,
                   std::size_t columns, std::size_t tile,
                   std::span<const std::uint32_t> expected) {
        std::fill(dst.begin(), dst.end(), sentinel);
        Kernel volatile kernel = candidate;
        const auto start = std::chrono::steady_clock::now();
        kernel(src, dst, rows, columns, tile);
        const auto end = std::chrono::steady_clock::now();
        if (!std::equal(dst.begin(), dst.end(), expected.begin(), expected.end()))
            throw std::runtime_error("Benchmark result differs from validated baseline");
        return std::chrono::duration<double, std::milli>(end - start).count();
    }
#endif
}

void testMatrixTranspose() {
    check({}, 0, 0, {});
    check({}, 0, 5, {});
    check({}, 7, 0, {});
    const std::array<std::uint32_t, 1> single{42};
    check(single, 1, 1, single);
    const std::array<std::uint32_t, 6> six{1, 2, 3, 4, 5, 6};
    check(six, 1, 6, six);
    check(six, 6, 1, six);
    const std::array<std::uint32_t, 6> twoByThree{1, 4, 2, 5, 3, 6};
    const std::array<std::uint32_t, 6> threeByTwo{1, 3, 5, 2, 4, 6};
    check(six, 2, 3, twoByThree);
    check(six, 3, 2, threeByTwo);
    const std::array<std::uint32_t, 15> fifteen{
        1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
    const std::array<std::uint32_t, 15> threeByFive{
        1, 6, 11, 2, 7, 12, 3, 8, 13, 4, 9, 14, 5, 10, 15};
    check(fifteen, 3, 5, threeByFive);
    check(threeByFive, 5, 3, fifteen);
    const std::array<std::uint32_t, 4> square{0, 0xffffffff, 0x80000000, 7};
    const std::array<std::uint32_t, 4> squareExpected{0, 0x80000000, 0xffffffff, 7};
    check(square, 2, 2, squareExpected);

    for (const bool empty : {false, true}) {
        std::array<std::uint32_t, 1> output{sentinel};
        bool rejected = false;
        try {
            transposeBlocked(std::span<const std::uint32_t>(single).first(empty ? 0 : 1),
                             std::span<std::uint32_t>(output).first(empty ? 0 : 1),
                             empty ? 0 : 1, 1, 0);
        }
        catch (const std::invalid_argument&) { rejected = true; }
        if (!rejected || output[0] != sentinel)
            throw std::runtime_error("tileSize=0 must throw before writing, even if empty");
    }
    std::cout << "All MatrixTranspose correctness tests passed.\n";
}

void benchmarkMatrixTranspose() {
#ifndef NDEBUG
    throw std::runtime_error("Use a Release build for the benchmark");
#else
    constexpr std::size_t repeats = 5;
    std::cout << "Median of 5 samples; warm buffers; allocation, reset and checks excluded.\n"
              << "rows,columns,tile,naive_ms,blocked_ms,naive_over_blocked\n";
    for (const auto [rows, columns] :
         {std::pair<std::size_t, std::size_t>{1024, 1024}, {2048, 2048}, {2049, 2051}}) {
        std::vector<std::uint32_t> src(rows * columns), dst(src.size()), expected(src.size());
        for (std::size_t i = 0; i < src.size(); ++i)
            src[i] = static_cast<std::uint32_t>(i * 17 + 13);
        // The student's baseline is used only after fixed-answer tests have passed.
        transposeNaive(src, expected, rows, columns);
        for (const std::size_t tile : {8, 16, 32, 64}) {
            (void)measure(baseline, src, dst, rows, columns, tile, expected);
            (void)measure(transposeBlocked, src, dst, rows, columns, tile, expected);
            std::array<double, repeats> naiveTimes{}, blockedTimes{};
            for (std::size_t sample = 0; sample < repeats; ++sample) {
                if (sample % 2 == 0) {
                    naiveTimes[sample] = measure(baseline, src, dst, rows, columns, tile, expected);
                    blockedTimes[sample] = measure(transposeBlocked, src, dst, rows, columns, tile, expected);
                }
                else {
                    blockedTimes[sample] = measure(transposeBlocked, src, dst, rows, columns, tile, expected);
                    naiveTimes[sample] = measure(baseline, src, dst, rows, columns, tile, expected);
                }
            }
            std::sort(naiveTimes.begin(), naiveTimes.end());
            std::sort(blockedTimes.begin(), blockedTimes.end());
            const double naive = naiveTimes[repeats / 2], blocked = blockedTimes[repeats / 2];
            std::cout << rows << ',' << columns << ',' << tile << ','
                      << std::fixed << std::setprecision(4) << naive << ',' << blocked << ',';
            if (blocked > 0) std::cout << naive / blocked;
            else std::cout << "n/a";
            std::cout << '\n';
        }
    }
#endif
}
