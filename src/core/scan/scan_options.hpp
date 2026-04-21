#pragma once

#include <chrono>
#include <cstdint>
#include <filesystem>
#include <vector>

namespace vk::core {

enum class ScanMode : std::uint8_t {
    Full,
    Incremental,
    QuickOverview
};

struct ScanFilter {
    std::vector<std::string> includeExtensions;
    std::vector<std::string> excludeExtensions;
    std::vector<std::filesystem::path> excludePaths;
    std::uint64_t minSizeBytes { 0 };
    std::uint64_t maxSizeBytes { 0 };
    std::chrono::system_clock::time_point olderThan {};
    std::chrono::system_clock::time_point newerThan {};
};

struct ScanOptions {
    std::vector<std::filesystem::path> roots;
    ScanMode mode { ScanMode::Full };
    ScanFilter filter;
    bool hashFiles { false };
    bool computePerceptualHashes { false };
    bool followSymlinks { false };
    bool useMftFastPath { true };
    bool persistCache { true };
    std::size_t workerThreads { 0 };
};

}
