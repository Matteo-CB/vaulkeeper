#pragma once

#include <cstdint>
#include <filesystem>
#include <memory>
#include <vector>

#include "fs/file_entry.hpp"
#include "util/bytes.hpp"
#include "util/time.hpp"

namespace vk::core {

struct ScanVolumeSummary {
    std::filesystem::path mountPoint;
    ByteCount totalBytes { 0 };
    ByteCount freeBytes { 0 };
    ByteCount scannedBytes { 0 };
    std::uint64_t fileCount { 0 };
    std::uint64_t directoryCount { 0 };
};

struct ScanStatistics {
    Duration elapsed;
    std::uint64_t filesVisited { 0 };
    std::uint64_t filesIndexed { 0 };
    std::uint64_t directoriesVisited { 0 };
    std::uint64_t bytesIndexed { 0 };
    std::uint64_t readBytes { 0 };
    std::uint32_t threadsUsed { 0 };
    std::uint32_t cacheHits { 0 };
    std::uint32_t cacheMisses { 0 };
    std::uint32_t errors { 0 };
};

struct ScanResult {
    TimePoint startedAt;
    TimePoint completedAt;
    ScanStatistics stats;
    std::vector<ScanVolumeSummary> volumes;
    std::shared_ptr<ScanNode> tree;
    std::vector<FileEntry> flatEntries;
};

}
