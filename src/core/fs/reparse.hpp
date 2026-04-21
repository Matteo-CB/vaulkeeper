#pragma once

#include <filesystem>

#include "util/result.hpp"

namespace vk::core {

[[nodiscard]] Result<void> createJunction(const std::filesystem::path& linkPath, const std::filesystem::path& target);
[[nodiscard]] Result<void> createSymbolicLink(const std::filesystem::path& linkPath, const std::filesystem::path& target, bool isDirectory);
[[nodiscard]] Result<std::filesystem::path> readReparsePoint(const std::filesystem::path& path);
[[nodiscard]] Result<void> removeReparsePoint(const std::filesystem::path& path);

}
