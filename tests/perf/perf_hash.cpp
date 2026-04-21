#include <array>

#include <catch2/benchmark/catch_benchmark_all.hpp>
#include <catch2/catch_all.hpp>

#include "hash/xxhash3.hpp"

TEST_CASE("xxhash3 throughput smoke", "[perf][hash][!benchmark]") {
    std::array<std::uint8_t, 1 << 20> buffer {};
    for (std::size_t i = 0; i < buffer.size(); ++i) {
        buffer[i] = static_cast<std::uint8_t>(i * 37u);
    }
    BENCHMARK("xxhash3 1 MiB") {
        return vk::core::xxhash3Bytes({ buffer.data(), buffer.size() });
    };
}
