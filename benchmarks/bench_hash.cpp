#include <array>

#include <benchmark/benchmark.h>

#include "hash/xxhash3.hpp"

static void BenchXxhash1M(benchmark::State& state) {
    std::array<std::uint8_t, 1 << 20> buffer {};
    for (std::size_t i = 0; i < buffer.size(); ++i) {
        buffer[i] = static_cast<std::uint8_t>(i);
    }
    for (auto _ : state) {
        benchmark::DoNotOptimize(vk::core::xxhash3Bytes({ buffer.data(), buffer.size() }));
    }
    state.SetBytesProcessed(static_cast<std::int64_t>(state.iterations()) * buffer.size());
}
BENCHMARK(BenchXxhash1M);
