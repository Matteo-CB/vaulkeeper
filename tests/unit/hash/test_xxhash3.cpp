#include <array>

#include <catch2/catch_all.hpp>

#include "hash/xxhash3.hpp"

using namespace vk::core;

TEST_CASE("xxhash3 is deterministic", "[hash]") {
    std::array<std::uint8_t, 5> bytes { 'h', 'e', 'l', 'l', 'o' };
    const auto a = xxhash3Bytes({ bytes.data(), bytes.size() });
    const auto b = xxhash3Bytes({ bytes.data(), bytes.size() });
    REQUIRE(a == b);
    REQUIRE(a.toHex().size() == 32);
}

TEST_CASE("xxhash3 differs between inputs", "[hash]") {
    std::array<std::uint8_t, 5> bytes1 { 'a', 'b', 'c', 'd', 'e' };
    std::array<std::uint8_t, 5> bytes2 { 'a', 'b', 'c', 'd', 'f' };
    const auto a = xxhash3Bytes({ bytes1.data(), bytes1.size() });
    const auto b = xxhash3Bytes({ bytes2.data(), bytes2.size() });
    REQUIRE_FALSE(a == b);
}
