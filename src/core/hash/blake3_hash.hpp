#pragma once

#include <array>
#include <cstdint>
#include <filesystem>
#include <span>
#include <string>

#include "util/result.hpp"

namespace vk::core {

using Blake3Digest = std::array<std::uint8_t, 32>;

[[nodiscard]] Blake3Digest blake3Bytes(std::span<const std::uint8_t> data) noexcept;
[[nodiscard]] Result<Blake3Digest> blake3File(const std::filesystem::path& path);
[[nodiscard]] std::string digestToHex(const Blake3Digest& digest);

}
