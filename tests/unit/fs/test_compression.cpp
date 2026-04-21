#include <filesystem>
#include <fstream>

#include <catch2/catch_all.hpp>

#include "fs/compression.hpp"

using namespace vk::core;

TEST_CASE("compression estimate returns positive ratio for compressible data", "[fs][compression]") {
    const auto dir = std::filesystem::temp_directory_path() / "vaulkeeper_comp_test";
    std::filesystem::create_directories(dir);
    const auto path = dir / "blob.bin";
    { std::ofstream { path, std::ios::binary } << std::string(8192, 'a'); }

    auto estimate = estimateCompression(path);
    REQUIRE(estimate.has_value());
    REQUIRE(estimate->originalBytes == 8192);
    REQUIRE(estimate->estimatedBytes <= estimate->originalBytes);
    REQUIRE(estimate->ratio >= 0.0);
}
