#pragma once

#include <array>
#include <cstdint>
#include <filesystem>
#include <span>

#include "util/result.hpp"

namespace vk::core {

using PerceptualHash = std::uint64_t;

struct PerceptualHashes {
    PerceptualHash phash { 0 };
    PerceptualHash dhash { 0 };
    PerceptualHash whash { 0 };
};

[[nodiscard]] std::uint32_t hammingDistance(PerceptualHash a, PerceptualHash b) noexcept;
[[nodiscard]] Result<PerceptualHashes> perceptualHashImage(const std::filesystem::path& imagePath);
[[nodiscard]] PerceptualHashes perceptualHashFromGrayscale(std::span<const std::uint8_t> pixels, std::uint32_t width, std::uint32_t height) noexcept;

}
