#include <catch2/catch_all.hpp>

#include "update_check.hpp"

using namespace vk::updater;

TEST_CASE("version comparison detects newer", "[updater]") {
    REQUIRE(newerThanCurrent("0.2.0", "0.1.0"));
    REQUIRE(newerThanCurrent("1.0.0", "0.9.9"));
    REQUIRE(newerThanCurrent("0.1.1", "0.1.0"));
    REQUIRE_FALSE(newerThanCurrent("0.1.0", "0.1.0"));
    REQUIRE_FALSE(newerThanCurrent("0.1.0", "0.2.0"));
}
