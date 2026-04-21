#include <catch2/catch_all.hpp>

#include "scheduler/cron.hpp"

using namespace vk::core;

TEST_CASE("cron parser accepts valid expressions", "[cron]") {
    auto every = parseCron("* * * * *");
    REQUIRE(every.has_value());
    REQUIRE(every->minutes.count() == 60);
    REQUIRE(every->hours.count() == 24);
}

TEST_CASE("cron parser rejects invalid expressions", "[cron]") {
    REQUIRE_FALSE(parseCron("").has_value());
    REQUIRE_FALSE(parseCron("too few").has_value());
    REQUIRE_FALSE(parseCron("99 * * * *").has_value());
}

TEST_CASE("cron computes next fire", "[cron]") {
    auto expr = parseCron("0 3 * * 0");
    REQUIRE(expr.has_value());
    auto fire = nextFire(*expr, std::chrono::system_clock::now());
    REQUIRE(fire.has_value());
}
