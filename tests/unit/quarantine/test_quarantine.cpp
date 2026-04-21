#include <chrono>
#include <filesystem>
#include <fstream>

#include <catch2/catch_all.hpp>

#include "quarantine/quarantine.hpp"

using namespace vk::core;

TEST_CASE("quarantine captures and restores", "[quarantine]") {
    const auto base = std::filesystem::temp_directory_path() / "vaulkeeper_q_test";
    std::filesystem::remove_all(base);

    const auto storage = base / "store";
    const auto file = base / "victim.bin";
    std::filesystem::create_directories(file.parent_path());
    { std::ofstream { file } << "data"; }

    Quarantine q;
    REQUIRE(q.initialize(storage, std::chrono::hours { 24 }).has_value());
    auto captured = q.capture(file, "test", "corr");
    REQUIRE(captured.has_value());
    REQUIRE_FALSE(std::filesystem::exists(file));

    REQUIRE(q.restore(captured->id).has_value());
    REQUIRE(std::filesystem::exists(file));
}
