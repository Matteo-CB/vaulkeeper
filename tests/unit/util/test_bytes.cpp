#include <catch2/catch_all.hpp>

#include "util/bytes.hpp"

using namespace vk::core;

TEST_CASE("formatBytes handles boundary values", "[util][bytes]") {
    REQUIRE(formatBytes(0) == "0 B");
    REQUIRE(formatBytes(1023) == "1023 B");
    REQUIRE(formatBytes(1_KiB) == "1.00 KiB");
    REQUIRE(formatBytes(1_MiB) == "1.00 MiB");
    REQUIRE(formatBytes(1_GiB) == "1.00 GiB");
}

TEST_CASE("byte literals compose", "[util][bytes]") {
    REQUIRE(2_KiB == 2048);
    REQUIRE(3_MiB == 3ull * 1024 * 1024);
    REQUIRE(4_GiB == 4ull * 1024 * 1024 * 1024);
}
