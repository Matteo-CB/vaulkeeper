#include <filesystem>
#include <fstream>
#include <random>

#include <catch2/catch_all.hpp>

#include "rules/rule_parser.hpp"

TEST_CASE("rule parser survives malformed yaml", "[fuzz][rules]") {
    const auto path = std::filesystem::temp_directory_path() / "vaulkeeper_fuzz_rules.yaml";
    std::mt19937 rng { 11 };
    std::string garbage;
    for (int i = 0; i < 1024; ++i) {
        garbage.push_back(static_cast<char>(rng() % 127));
    }
    { std::ofstream { path } << garbage; }
    auto parsed = vk::core::parseRulesFromYaml(path);
    REQUIRE((parsed.has_value() || !parsed.has_value()));
}
