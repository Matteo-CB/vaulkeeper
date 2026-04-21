#include <catch2/catch_all.hpp>

#include "fs/path.hpp"

using namespace vk::core;

TEST_CASE("reserved windows name detection", "[fs][path]") {
    REQUIRE(isReservedWindowsName("CON"));
    REQUIRE(isReservedWindowsName("nul.txt"));
    REQUIRE_FALSE(isReservedWindowsName("readme.md"));
    REQUIRE_FALSE(isReservedWindowsName(""));
}

TEST_CASE("path utf8 round trip", "[fs][path]") {
    const std::string input = "C:/Users/Kutxyt/Documents/noel.txt";
    const auto path = fromUtf8(input);
    REQUIRE(toUtf8(path) == input);
}

TEST_CASE("extended path prefix", "[fs][path]") {
    const auto result = toExtendedPath(std::filesystem::path { "C:/foo/bar" });
    REQUIRE(result.rfind(L"\\\\?\\", 0) == 0);
}
