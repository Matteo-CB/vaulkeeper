#include <filesystem>
#include <fstream>

#include <catch2/catch_all.hpp>

#include "uninstall/residue_scanner.hpp"

using namespace vk::core;

TEST_CASE("residue scanner surfaces directory matches", "[uninstall][residue]") {
    const auto root = std::filesystem::temp_directory_path() / "vaulkeeper_residue";
    std::filesystem::remove_all(root);
    std::filesystem::create_directories(root / "Acme Corp");
    std::ofstream { root / "Acme Corp" / "settings.dat" } << "body";

    auto report = scanResiduesForApplication("acme_app", "Acme Corp", root.generic_string());
    REQUIRE(report.has_value());
    REQUIRE_FALSE(report->filesystem.empty());
    REQUIRE(report->totalBytes >= 4);
}
