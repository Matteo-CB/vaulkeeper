#pragma once

#include <chrono>
#include <filesystem>
#include <string>
#include <vector>

#include "util/result.hpp"

namespace vk::core {

struct QuarantineConfig {
    std::chrono::hours retention { 24 * 30 };
    std::uint64_t maxTotalBytes { 0 };
    bool compressStoredItems { true };
};

struct ScanConfig {
    bool useMftFastPath { true };
    bool hashLargeFiles { false };
    std::uint64_t largeFileThresholdBytes { 256ULL * 1024ULL * 1024ULL };
    std::size_t workerThreads { 0 };
    bool persistCache { true };
};

struct UiConfig {
    std::string theme { "dark" };
    std::string density { "normal" };
    std::string locale { "en" };
    bool reducedMotion { false };
};

struct TelemetryConfig {
    bool enabled { false };
    std::string endpoint;
};

struct AppConfig {
    QuarantineConfig quarantine;
    ScanConfig scan;
    UiConfig ui;
    TelemetryConfig telemetry;

    [[nodiscard]] static Result<AppConfig> load(const std::filesystem::path& path);
    [[nodiscard]] Result<void> save(const std::filesystem::path& path) const;
    [[nodiscard]] static AppConfig defaults();
};

}
