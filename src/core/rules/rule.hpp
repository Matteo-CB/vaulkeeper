#pragma once

#include <chrono>
#include <cstdint>
#include <string>
#include <variant>
#include <vector>

namespace vk::core {

enum class RuleAction : std::uint8_t {
    DeleteToQuarantine,
    PermanentDelete,
    Compress,
    MoveTo,
    TagWith,
    RunShell,
    Notify
};

enum class RuleTriggerKind : std::uint8_t {
    Cron,
    FreeSpaceBelow,
    OnBoot,
    OnLogin,
    OnExternalDisk,
    Manual
};

struct RuleTrigger {
    RuleTriggerKind kind { RuleTriggerKind::Manual };
    std::string cronExpression;
    std::uint64_t freeSpaceThresholdBytes { 0 };
};

struct RuleCondition {
    std::string path;
    std::uint64_t minAgeDays { 0 };
    std::uint64_t minSizeBytes { 0 };
    std::uint64_t maxSizeBytes { 0 };
    std::vector<std::string> extensions;
    std::string matcher;
};

struct Rule {
    std::string id;
    std::string description;
    bool enabled { true };
    bool dryRun { true };
    RuleTrigger trigger;
    RuleCondition condition;
    RuleAction action { RuleAction::DeleteToQuarantine };
    std::string actionTarget;
    std::uint32_t priority { 100 };
};

}
