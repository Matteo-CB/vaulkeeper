#include <filesystem>
#include <fstream>

#include <catch2/catch_all.hpp>

#include "rules/rule_engine.hpp"

using namespace vk::core;

TEST_CASE("rule engine dry run reports matches", "[integration][rules]") {
    const auto root = std::filesystem::temp_directory_path() / "vaulkeeper_rule_itest";
    std::filesystem::remove_all(root);
    std::filesystem::create_directories(root);
    std::ofstream { root / "a.log" } << std::string(8192, 'a');
    std::ofstream { root / "b.log" } << std::string(1024, 'b');

    Rule rule;
    rule.id = "logs.cleanup";
    rule.condition.path = root.string();
    rule.condition.extensions = { ".log" };
    rule.condition.minSizeBytes = 2048;

    RuleEngine engine { nullptr, nullptr };
    auto report = engine.execute(rule, true);
    REQUIRE(report.has_value());
    REQUIRE(report->actedCount == 1);
    REQUIRE(report->reclaimedBytes >= 8192);
}
