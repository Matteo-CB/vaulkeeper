#pragma once

#include <memory>
#include <vector>

#include "rules/rule.hpp"
#include "util/result.hpp"

namespace vk::core {

class Quarantine;
class Journal;

struct RuleEvaluation {
    std::string ruleId;
    std::vector<std::filesystem::path> matchedPaths;
    std::uint64_t totalBytes { 0 };
};

struct RuleExecutionReport {
    std::string ruleId;
    bool dryRun { true };
    std::size_t actedCount { 0 };
    std::uint64_t reclaimedBytes { 0 };
    std::vector<std::string> errors;
};

class RuleEngine {
public:
    RuleEngine(std::shared_ptr<Quarantine> quarantine, std::shared_ptr<Journal> journal);

    [[nodiscard]] Result<RuleEvaluation> evaluate(const Rule& rule) const;
    [[nodiscard]] Result<RuleExecutionReport> execute(const Rule& rule, bool dryRun);

private:
    std::shared_ptr<Quarantine> quarantine;
    std::shared_ptr<Journal> journal;
};

}
