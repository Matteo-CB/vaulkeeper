#pragma once

#include <chrono>
#include <cstdint>
#include <filesystem>
#include <optional>
#include <string>
#include <vector>

#include "util/result.hpp"
#include "util/time.hpp"

struct sqlite3;

namespace vk::core {

enum class JournalEntryKind : std::uint16_t {
    ScanStarted,
    ScanCompleted,
    FileDeleted,
    FileRestored,
    QuarantineAdded,
    QuarantineRestored,
    QuarantinePurged,
    UninstallPerformed,
    RuleExecuted,
    ConfigChanged,
    PluginLoaded,
    Error
};

struct JournalEntry {
    std::uint64_t id { 0 };
    JournalEntryKind kind { JournalEntryKind::Error };
    TimePoint timestamp;
    std::string actor;
    std::string subject;
    std::string detail;
    std::int64_t sizeBytes { 0 };
    std::string correlationId;
};

class Journal {
public:
    Journal();
    ~Journal();

    VK_NONCOPYABLE(Journal);
    VK_NONMOVABLE(Journal);

    [[nodiscard]] Result<void> open(const std::filesystem::path& dbPath);
    void close() noexcept;

    [[nodiscard]] Result<std::uint64_t> record(const JournalEntry& entry);
    [[nodiscard]] Result<std::vector<JournalEntry>> query(std::optional<TimePoint> since, std::optional<TimePoint> until, std::size_t limit) const;
    [[nodiscard]] Result<std::vector<JournalEntry>> byCorrelation(std::string_view correlationId) const;
    [[nodiscard]] Result<void> vacuum();

private:
    sqlite3* db { nullptr };
};

}
