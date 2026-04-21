#include <random>

#include <catch2/catch_all.hpp>

#include "scheduler/cron.hpp"

using namespace vk::core;

TEST_CASE("cron parser refuses random junk", "[fuzz][cron]") {
    std::mt19937 rng { 7 };
    std::uniform_int_distribution<int> chars { 32, 126 };

    for (int i = 0; i < 64; ++i) {
        std::string random;
        for (int j = 0; j < 16; ++j) {
            random.push_back(static_cast<char>(chars(rng)));
        }
        auto parsed = parseCron(random);
        REQUIRE((parsed.has_value() || !parsed.has_value()));
    }
}
