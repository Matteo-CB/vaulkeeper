#pragma once

#include <cstdint>
#include <filesystem>

#include "util/result.hpp"

namespace vk::core {

enum class CompressionKind : std::uint8_t { NtfsDefault, Lzx };

struct CompressionEstimate {
    std::uint64_t originalBytes { 0 };
    std::uint64_t estimatedBytes { 0 };
    double ratio { 0.0 };
};

[[nodiscard]] Result<void> compressPathNtfs(const std::filesystem::path& path, CompressionKind kind = CompressionKind::NtfsDefault);
[[nodiscard]] Result<void> decompressPathNtfs(const std::filesystem::path& path);
[[nodiscard]] Result<CompressionEstimate> estimateCompression(const std::filesystem::path& path);

}
