#pragma once

#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

#include "util/result.hpp"

namespace vk::core {

enum class ResidueConfidence : std::uint8_t {
    VeryLikely,
    Likely,
    Possible,
    NeedsReview
};

struct ResidueCandidate {
    std::string source;
    std::filesystem::path path;
    std::string registryPath;
    std::uint64_t sizeBytes { 0 };
    ResidueConfidence confidence { ResidueConfidence::Possible };
    std::string rationale;
};

struct ResidueReport {
    std::string applicationId;
    std::vector<ResidueCandidate> filesystem;
    std::vector<ResidueCandidate> registry;
    std::uint64_t totalBytes { 0 };
};

[[nodiscard]] Result<ResidueReport> scanResiduesForApplication(const std::string& applicationId, std::string_view publisher, std::string_view installLocation);

}
