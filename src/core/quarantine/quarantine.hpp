#pragma once

#include <chrono>
#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

#include "util/result.hpp"
#include "util/time.hpp"

namespace vk::core {

struct QuarantineItem {
    std::string id;
    std::filesystem::path originalPath;
    std::filesystem::path storagePath;
    std::uint64_t originalSizeBytes { 0 };
    std::uint64_t storedSizeBytes { 0 };
    TimePoint capturedAt;
    TimePoint expiresAt;
    bool compressed { false };
    std::string reason;
    std::string correlationId;
};

class Quarantine {
public:
    Quarantine() = default;

    VK_NONCOPYABLE(Quarantine);
    VK_NONMOVABLE(Quarantine);

    [[nodiscard]] Result<void> initialize(std::filesystem::path storageRoot, std::chrono::hours retention);

    [[nodiscard]] Result<QuarantineItem> capture(const std::filesystem::path& source, std::string reason, std::string correlationId);
    [[nodiscard]] Result<void> restore(std::string_view itemId);
    [[nodiscard]] Result<void> discard(std::string_view itemId);
    [[nodiscard]] Result<std::size_t> purgeExpired(TimePoint now = SystemClock::now());

    [[nodiscard]] Result<std::vector<QuarantineItem>> list() const;
    [[nodiscard]] std::uint64_t totalStoredBytes() const;

private:
    std::filesystem::path root;
    std::chrono::hours retention { 24 * 30 };
    std::uint64_t storedBytes { 0 };
    std::vector<QuarantineItem> items;

    [[nodiscard]] std::filesystem::path indexFile() const;
    [[nodiscard]] Result<void> persistIndex() const;
    [[nodiscard]] Result<void> loadIndex();
};

}
