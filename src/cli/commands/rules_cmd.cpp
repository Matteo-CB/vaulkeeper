#include "commands/rules_cmd.hpp"

#include <fmt/core.h>

#include "config/paths.hpp"
#include "rules/rule_engine.hpp"
#include "rules/rule_parser.hpp"

namespace vk::cli {

int RulesCommand::run(std::span<const std::string> args, const CommandContext& ctx) {
    const auto paths = core::Paths::resolveDefault(false);
    const auto rulesPath = paths.rulesDir / "rules.yaml";

    auto rules = core::parseRulesFromYaml(rulesPath);
    if (!rules) {
        fmt::print(stderr, "rules parse failed: {}\n", rules.error().message);
        return 3;
    }

    if (args.empty() || args.front() == "list") {
        for (const auto& rule : *rules) {
            fmt::print("{}\t{}\tenabled={}\n", rule.id, rule.description, rule.enabled);
        }
        return 0;
    }
    if (args.front() == "run" && args.size() == 2) {
        core::RuleEngine engine { nullptr, nullptr };
        const auto it = std::find_if(rules->begin(), rules->end(),
                                     [&](const core::Rule& r) { return r.id == args[1]; });
        if (it == rules->end()) {
            fmt::print(stderr, "rule not found: {}\n", args[1]);
            return 2;
        }
        auto report = engine.execute(*it, ctx.dryRun);
        if (!report) { fmt::print(stderr, "{}\n", report.error().message); return 3; }
        fmt::print("rule {} acted={} reclaimed={}\n", report->ruleId, report->actedCount, report->reclaimedBytes);
        return 0;
    }

    fmt::print(stderr, "usage: vk rules [list|run <id>]\n");
    return 2;
}

}
