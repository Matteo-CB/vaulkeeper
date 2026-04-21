#include <filesystem>
#include <fstream>

#include <catch2/catch_all.hpp>

#include "duplicates/duplicate_finder.hpp"

using namespace vk::core;

TEST_CASE("duplicate finder clusters identical files", "[duplicates]") {
    const auto base = std::filesystem::temp_directory_path() / "vaulkeeper_dup_test";
    std::filesystem::remove_all(base);
    std::filesystem::create_directories(base);

    const auto content = std::string(8192, 'x');
    const auto pathA = base / "a.bin";
    const auto pathB = base / "b.bin";
    const auto pathC = base / "c.bin";
    {
        std::ofstream { pathA } << content;
        std::ofstream { pathB } << content;
        std::ofstream { pathC } << std::string(8192, 'y');
    }

    DuplicateOptions options;
    options.minSize = 0;
    auto clusters = findDuplicates({ pathA, pathB, pathC }, options);
    REQUIRE(clusters.has_value());
    REQUIRE(clusters->size() == 1);
    REQUIRE(clusters->front().files.size() == 2);
}
