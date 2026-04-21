#include <array>
#include <random>

#include <catch2/catch_all.hpp>

#include "ipc/message.hpp"

using namespace vk::core;

TEST_CASE("ipc deserialize never crashes on fuzzed input", "[fuzz][ipc]") {
    std::array<std::uint8_t, 32> key {};
    for (std::size_t i = 0; i < key.size(); ++i) { key[i] = static_cast<std::uint8_t>(i); }

    std::mt19937 rng { 23 };
    for (int i = 0; i < 256; ++i) {
        std::vector<std::uint8_t> random(64 + (rng() % 512));
        for (auto& byte : random) { byte = static_cast<std::uint8_t>(rng() & 0xFF); }
        auto parsed = deserialize(random, key.data());
        REQUIRE((parsed.has_value() || !parsed.has_value()));
    }
}
