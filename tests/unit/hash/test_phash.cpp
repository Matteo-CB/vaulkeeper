#include <vector>

#include <catch2/catch_all.hpp>

#include "hash/phash.hpp"

using namespace vk::core;

TEST_CASE("hamming distance counts differing bits", "[hash][phash]") {
    REQUIRE(hammingDistance(0ull, 0ull) == 0);
    REQUIRE(hammingDistance(0ull, 0xFFull) == 8);
    REQUIRE(hammingDistance(0xF0F0F0F0F0F0F0F0ull, 0x0F0F0F0F0F0F0F0Full) == 64);
}

TEST_CASE("perceptual hash returns zero for empty", "[hash][phash]") {
    std::vector<std::uint8_t> empty;
    const auto hashes = perceptualHashFromGrayscale(empty, 0, 0);
    REQUIRE(hashes.phash == 0);
    REQUIRE(hashes.dhash == 0);
    REQUIRE(hashes.whash == 0);
}
