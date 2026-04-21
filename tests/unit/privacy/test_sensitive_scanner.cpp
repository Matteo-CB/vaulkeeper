#include <filesystem>
#include <fstream>

#include <catch2/catch_all.hpp>

#include "privacy/sensitive_scanner.hpp"

using namespace vk::core;

TEST_CASE("sensitive scanner detects emails", "[privacy]") {
    const auto dir = std::filesystem::temp_directory_path() / "vaulkeeper_priv_test";
    std::filesystem::create_directories(dir);
    const auto path = dir / "notes.txt";
    { std::ofstream { path } << "contact: foo.bar@example.com\n"; }

    SensitiveScanOptions opts;
    auto matches = scanForSensitiveData({ path }, opts);
    REQUIRE(matches.has_value());
    REQUIRE_FALSE(matches->empty());
}
