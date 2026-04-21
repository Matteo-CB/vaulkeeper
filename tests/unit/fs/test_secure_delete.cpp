#include <filesystem>
#include <fstream>

#include <catch2/catch_all.hpp>

#include "fs/secure_delete.hpp"

using namespace vk::core;

TEST_CASE("secure delete zero pass removes file", "[fs][secure_delete]") {
    const auto root = std::filesystem::temp_directory_path() / "vaulkeeper_secdel_test";
    std::filesystem::create_directories(root);
    const auto path = root / "victim.bin";
    { std::ofstream { path, std::ios::binary } << std::string(1024, 'X'); }

    SecureDeleteOptions options;
    options.standard = SecureDeleteStandard::Zero;
    const auto result = secureDeleteFile(path, options);
    REQUIRE(result.has_value());
    REQUIRE(*result == 1024);
    REQUIRE_FALSE(std::filesystem::exists(path));
}

TEST_CASE("secure delete dod pass preserves size before removal", "[fs][secure_delete]") {
    const auto root = std::filesystem::temp_directory_path() / "vaulkeeper_secdel_dod";
    std::filesystem::create_directories(root);
    const auto path = root / "keep.bin";
    const std::string payload(2048, 'k');
    { std::ofstream { path, std::ios::binary } << payload; }

    SecureDeleteOptions options;
    options.standard = SecureDeleteStandard::Dod522022M;
    options.removeAfterOverwrite = false;
    const auto result = secureDeleteFile(path, options);
    REQUIRE(result.has_value());
    REQUIRE(std::filesystem::exists(path));
    REQUIRE(std::filesystem::file_size(path) == payload.size());
    std::filesystem::remove(path);
}
