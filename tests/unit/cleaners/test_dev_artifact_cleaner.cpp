#include <filesystem>
#include <fstream>

#include <catch2/catch_all.hpp>

#include "cleaners/dev_artifact_cleaner.hpp"

using namespace vk::core;

TEST_CASE("dev artifact cleaner flags node_modules", "[cleaners][dev]") {
    const auto root = std::filesystem::temp_directory_path() / "vaulkeeper_dev_art";
    std::filesystem::remove_all(root);
    std::filesystem::create_directories(root / "project" / "node_modules" / "left-pad");
    std::ofstream { root / "project" / "node_modules" / "left-pad" / "index.js" } << std::string(512, '/');

    CleanerContext ctx;
    ctx.userProfile = root;
    DevArtifactCleaner cleaner;
    auto report = cleaner.scan(ctx);
    REQUIRE(report.has_value());
    REQUIRE(report->totalReclaimableBytes >= 512);
    REQUIRE_FALSE(report->candidates.empty());
}
