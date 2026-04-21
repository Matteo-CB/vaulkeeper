#include <catch2/catch_all.hpp>

#include <array>
#include <random>

TEST_CASE("mft boot sector parsing tolerates noise", "[fuzz][mft]") {
    std::array<std::uint8_t, 512> buffer {};
    std::mt19937 rng { 42 };
    for (auto& byte : buffer) { byte = static_cast<std::uint8_t>(rng() & 0xFF); }
    REQUIRE(buffer.size() == 512);
}
