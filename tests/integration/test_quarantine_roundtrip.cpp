#include <filesystem>
#include <fstream>

#include <catch2/catch_all.hpp>

#include "quarantine/quarantine.hpp"

using namespace vk::core;

TEST_CASE("quarantine purges expired items", "[integration][quarantine]") {
    const auto base = std::filesystem::temp_directory_path() / "vaulkeeper_q_itest";
    std::filesystem::remove_all(base);

    const auto storage = base / "store";
    const auto file = base / "big.bin";
    std::filesystem::create_directories(file.parent_path());
    { std::ofstream { file } << std::string(4096, 'z'); }

    Quarantine q;
    REQUIRE(q.initialize(storage, std::chrono::hours { 0 }).has_value());
    auto captured = q.capture(file, "test", "corr");
    REQUIRE(captured.has_value());
    auto purged = q.purgeExpired();
    REQUIRE(purged.has_value());
    REQUIRE(*purged == 1);
}
