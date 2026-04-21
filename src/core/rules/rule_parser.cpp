#include "rules/rule_parser.hpp"

#include <fstream>

#include <yaml-cpp/yaml.h>

namespace vk::core {

namespace {

RuleTriggerKind triggerFromString(const std::string& s) {
    if (s == "cron") return RuleTriggerKind::Cron;
    if (s == "free_space_below") return RuleTriggerKind::FreeSpaceBelow;
    if (s == "on_boot") return RuleTriggerKind::OnBoot;
    if (s == "on_login") return RuleTriggerKind::OnLogin;
    if (s == "on_external_disk") return RuleTriggerKind::OnExternalDisk;
    return RuleTriggerKind::Manual;
}

std::string triggerToString(RuleTriggerKind kind) {
    switch (kind) {
        case RuleTriggerKind::Cron: return "cron";
        case RuleTriggerKind::FreeSpaceBelow: return "free_space_below";
        case RuleTriggerKind::OnBoot: return "on_boot";
        case RuleTriggerKind::OnLogin: return "on_login";
        case RuleTriggerKind::OnExternalDisk: return "on_external_disk";
        case RuleTriggerKind::Manual: return "manual";
    }
    return "manual";
}

RuleAction actionFromString(const std::string& s) {
    if (s == "delete_to_quarantine") return RuleAction::DeleteToQuarantine;
    if (s == "permanent_delete") return RuleAction::PermanentDelete;
    if (s == "compress") return RuleAction::Compress;
    if (s == "move_to") return RuleAction::MoveTo;
    if (s == "tag_with") return RuleAction::TagWith;
    if (s == "run_shell") return RuleAction::RunShell;
    if (s == "notify") return RuleAction::Notify;
    return RuleAction::DeleteToQuarantine;
}

std::string actionToString(RuleAction action) {
    switch (action) {
        case RuleAction::DeleteToQuarantine: return "delete_to_quarantine";
        case RuleAction::PermanentDelete: return "permanent_delete";
        case RuleAction::Compress: return "compress";
        case RuleAction::MoveTo: return "move_to";
        case RuleAction::TagWith: return "tag_with";
        case RuleAction::RunShell: return "run_shell";
        case RuleAction::Notify: return "notify";
    }
    return "notify";
}

}

Result<std::vector<Rule>> parseRulesFromYaml(const std::filesystem::path& path) {
    try {
        auto doc = YAML::LoadFile(path.string());
        std::vector<Rule> rules;
        if (!doc["rules"]) { return rules; }
        for (const auto& item : doc["rules"]) {
            Rule r;
            r.id = item["id"].as<std::string>("");
            r.description = item["description"].as<std::string>("");
            r.enabled = item["enabled"].as<bool>(true);
            r.dryRun = item["dry_run"].as<bool>(true);
            r.priority = item["priority"].as<std::uint32_t>(100);

            if (item["trigger"]) {
                const auto& t = item["trigger"];
                r.trigger.kind = triggerFromString(t["kind"].as<std::string>("manual"));
                r.trigger.cronExpression = t["cron"].as<std::string>("");
                r.trigger.freeSpaceThresholdBytes = t["free_space_below_bytes"].as<std::uint64_t>(0);
            }
            if (item["condition"]) {
                const auto& c = item["condition"];
                r.condition.path = c["path"].as<std::string>("");
                r.condition.minAgeDays = c["min_age_days"].as<std::uint64_t>(0);
                r.condition.minSizeBytes = c["min_size_bytes"].as<std::uint64_t>(0);
                r.condition.maxSizeBytes = c["max_size_bytes"].as<std::uint64_t>(0);
                r.condition.matcher = c["matcher"].as<std::string>("");
                if (c["extensions"]) {
                    for (const auto& e : c["extensions"]) {
                        r.condition.extensions.push_back(e.as<std::string>());
                    }
                }
            }
            r.action = actionFromString(item["action"].as<std::string>("delete_to_quarantine"));
            r.actionTarget = item["action_target"].as<std::string>("");
            rules.push_back(std::move(r));
        }
        return rules;
    } catch (const YAML::Exception& e) {
        return fail(ErrorCode::CorruptedData, e.what());
    }
}

Result<void> writeRulesToYaml(const std::filesystem::path& path, const std::vector<Rule>& rules) {
    YAML::Node root;
    auto list = YAML::Node(YAML::NodeType::Sequence);
    for (const auto& r : rules) {
        YAML::Node item;
        item["id"] = r.id;
        item["description"] = r.description;
        item["enabled"] = r.enabled;
        item["dry_run"] = r.dryRun;
        item["priority"] = r.priority;
        item["trigger"]["kind"] = triggerToString(r.trigger.kind);
        item["trigger"]["cron"] = r.trigger.cronExpression;
        item["trigger"]["free_space_below_bytes"] = r.trigger.freeSpaceThresholdBytes;
        item["condition"]["path"] = r.condition.path;
        item["condition"]["min_age_days"] = r.condition.minAgeDays;
        item["condition"]["min_size_bytes"] = r.condition.minSizeBytes;
        item["condition"]["max_size_bytes"] = r.condition.maxSizeBytes;
        item["condition"]["matcher"] = r.condition.matcher;
        item["condition"]["extensions"] = r.condition.extensions;
        item["action"] = actionToString(r.action);
        item["action_target"] = r.actionTarget;
        list.push_back(item);
    }
    root["rules"] = list;

    std::error_code ec;
    std::filesystem::create_directories(path.parent_path(), ec);
    std::ofstream os(path);
    if (!os) { return fail(ErrorCode::IoError, "cannot write rules"); }
    os << root;
    return ok();
}

}
