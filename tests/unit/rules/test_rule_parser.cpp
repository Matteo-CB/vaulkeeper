#include <filesystem>
#include <fstream>

#include <catch2/catch_all.hpp>

#include "rules/rule_parser.hpp"

using namespace vk::core;

TEST_CASE("rule parser round trips", "[rules]") {
    const auto dir = std::filesystem::temp_directory_path() / "vaulkeeper_rule_test";
    std::filesystem::create_directories(dir);
    const auto path = dir / "rules.yaml";

    std::vector<Rule> in;
    Rule r;
    r.id = "cleanup.downloads";
    r.description = "Archive old downloads";
    r.enabled = true;
    r.dryRun = false;
    r.condition.path = "C:/Users/test/Downloads";
    r.condition.minAgeDays = 60;
    r.condition.extensions = { ".zip" };
    r.action = RuleAction::Compress;
    in.push_back(r);

    REQUIRE(writeRulesToYaml(path, in).has_value());
    auto parsed = parseRulesFromYaml(path);
    REQUIRE(parsed.has_value());
    REQUIRE(parsed->size() == 1);
    REQUIRE(parsed->front().id == "cleanup.downloads");
    REQUIRE(parsed->front().condition.minAgeDays == 60);
}
