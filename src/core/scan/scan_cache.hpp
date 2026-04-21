#pragma once

#include <cstdint>
#include <filesystem>
#include <optional>

#include "fs/file_entry.hpp"
#include "util/result.hpp"

struct sqlite3;

namespace vk::core {

struct ScanCacheEntry {
    std::uint64_t recordNumber { 0 };
    std::uint32_t usnVersion { 0 };
    std::uint64_t logicalSize { 0 };
    std::int64_t modifiedNs { 0 };
    std::string pathKey;
};

class ScanCache {
public:
    ScanCache();
    ~ScanCache();

    VK_NONCOPYABLE(ScanCache);
    VK_NONMOVABLE(ScanCache);

    [[nodiscard]] Result<void> open(const std::filesystem::path& dbPath);
    void close() noexcept;

    [[nodiscard]] Result<void> upsert(const ScanCacheEntry& entry);
    [[nodiscard]] Result<std::optional<ScanCacheEntry>> findByRecord(std::uint64_t recordNumber) const;
    [[nodiscard]] Result<void> vacuum();

private:
    sqlite3* db { nullptr };
};

}
