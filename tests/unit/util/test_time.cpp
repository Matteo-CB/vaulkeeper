#include <catch2/catch_all.hpp>

#include "util/time.hpp"

using namespace vk::core;

TEST_CASE("time conversion round trips", "[util][time]") {
    const auto now = SystemClock::now();
    const auto ns = toUnixNanos(now);
    const auto restored = fromUnixNanos(ns);
    REQUIRE(std::chrono::duration_cast<std::chrono::microseconds>(now - restored).count() <= 1);
}

TEST_CASE("formatDuration picks sensible units", "[util][time]") {
    using namespace std::chrono_literals;
    REQUIRE(formatDuration(12us).find("us") != std::string::npos);
    REQUIRE(formatDuration(5ms).find("ms") != std::string::npos);
    REQUIRE(formatDuration(5s).find("s") != std::string::npos);
}
