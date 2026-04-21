#include "rules/rule_engine.hpp"

#include <algorithm>
#include <filesystem>

#include <fmt/format.h>
#include <spdlog/spdlog.h>

#include "journal/journal.hpp"
#include "quarantine/quarantine.hpp"

namespace vk::core {

namespace {

bool matchesExtension(const std::filesystem::path& p, const std::vector<std::string>& extensions) {
    if (extensions.empty()) { return true; }
    const auto ext = p.extension().generic_string();
    return std::any_of(extensions.begin(), extensions.end(),
                      [&](const std::string& e) { return ext == e || ext == ("." + e); });
}

bool olderThanDays(const std::filesystem::path& p, std::uint64_t days) {
    if (days == 0) { return true; }
    std::error_code ec;
    const auto mtime = std::filesystem::last_write_time(p, ec);
    if (ec) { return false; }
    const auto now = std::filesystem::file_time_type::clock::now();
    const auto diff = std::chrono::duration_cast<std::chrono::hours>(now - mtime).count();
    return diff >= static_cast<std::int64_t>(days * 24);
}

}

RuleEngine::RuleEngine(std::shared_ptr<Quarantine> q, std::shared_ptr<Journal> j)
    : quarantine(std::move(q)), journal(std::move(j)) {}

Result<RuleEvaluation> RuleEngine::evaluate(const Rule& rule) const {
    RuleEvaluation eval;
    eval.ruleId = rule.id;

    if (rule.condition.path.empty()) {
        return fail(ErrorCode::InvalidArgument, "rule condition path missing");
    }

    std::error_code ec;
    for (auto it = std::filesystem::recursive_directory_iterator(rule.condition.path, ec);
         it != std::filesystem::recursive_directory_iterator(); ++it) {
        if (ec) { ec.clear(); continue; }
        if (!it->is_regular_file(ec)) { continue; }
        const auto size = it->file_size(ec);
        if (rule.condition.minSizeBytes != 0 && size < rule.condition.minSizeBytes) { continue; }
        if (rule.condition.maxSizeBytes != 0 && size > rule.condition.maxSizeBytes) { continue; }
        if (!matchesExtension(it->path(), rule.condition.extensions)) { continue; }
        if (!olderThanDays(it->path(), rule.condition.minAgeDays)) { continue; }
        eval.matchedPaths.push_back(it->path());
        eval.totalBytes += size;
    }

    return eval;
}

Result<RuleExecutionReport> RuleEngine::execute(const Rule& rule, bool dryRun) {
    RuleExecutionReport report;
    report.ruleId = rule.id;
    report.dryRun = dryRun;

    auto evaluation = evaluate(rule);
    if (!evaluation) {
        return fail(evaluation.error());
    }

    for (const auto& path : evaluation->matchedPaths) {
        std::error_code ec;
        const auto size = std::filesystem::file_size(path, ec);
        if (ec) { report.errors.push_back(path.generic_string()); continue; }

        if (!dryRun && quarantine && rule.action == RuleAction::DeleteToQuarantine) {
            auto captured = quarantine->capture(path, fmt::format("rule:{}", rule.id), rule.id);
            if (!captured) {
                report.errors.push_back(path.generic_string());
                continue;
            }
        }

        report.reclaimedBytes += size;
        ++report.actedCount;
    }

    if (journal) {
        JournalEntry entry;
        entry.kind = JournalEntryKind::RuleExecuted;
        entry.timestamp = SystemClock::now();
        entry.actor = "rule_engine";
        entry.subject = rule.id;
        entry.detail = fmt::format("acted={} reclaimed={} dry_run={}", report.actedCount, report.reclaimedBytes, report.dryRun);
        entry.sizeBytes = static_cast<std::int64_t>(report.reclaimedBytes);
        entry.correlationId = rule.id;
        (void)journal->record(entry);
    }

    spdlog::info("rule {} acted={} reclaimed={}B dry_run={}", rule.id, report.actedCount, report.reclaimedBytes, report.dryRun);
    return report;
}

}
