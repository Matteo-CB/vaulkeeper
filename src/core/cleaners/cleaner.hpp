#pragma once

#include <cstdint>
#include <filesystem>
#include <memory>
#include <string>
#include <vector>

#include "util/result.hpp"

namespace vk::core {

struct CleanerCandidate {
    std::filesystem::path path;
    std::uint64_t reclaimableBytes { 0 };
    std::string reason;
    std::string category;
};

struct CleanerReport {
    std::string cleanerId;
    std::vector<CleanerCandidate> candidates;
    std::uint64_t totalReclaimableBytes { 0 };
};

struct CleanerContext {
    bool dryRun { true };
    std::filesystem::path userProfile;
    std::filesystem::path localAppData;
    std::filesystem::path appData;
    std::filesystem::path temp;
    std::filesystem::path programFiles;
    std::filesystem::path programFilesX86;
};

class Cleaner {
public:
    virtual ~Cleaner() = default;

    [[nodiscard]] virtual std::string id() const = 0;
    [[nodiscard]] virtual std::string displayName() const = 0;
    [[nodiscard]] virtual Result<CleanerReport> scan(const CleanerContext& context) = 0;
};

using CleanerPtr = std::shared_ptr<Cleaner>;

}
