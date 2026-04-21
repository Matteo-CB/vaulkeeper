#include <array>

#include <catch2/catch_all.hpp>

#include "hash/blake3_hash.hpp"

using namespace vk::core;

TEST_CASE("blake3 is deterministic", "[hash]") {
    std::array<std::uint8_t, 5> bytes { 'h', 'e', 'l', 'l', 'o' };
    const auto a = blake3Bytes({ bytes.data(), bytes.size() });
    const auto b = blake3Bytes({ bytes.data(), bytes.size() });
    REQUIRE(a == b);
    REQUIRE(digestToHex(a).size() == 64);
}
