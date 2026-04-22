#pragma once

#include <filesystem>
#include <string>

#include "util/result.hpp"

namespace vk::updater {

[[nodiscard]] core::Result<void> verifyAuthenticode(const std::filesystem::path& binary);
[[nodiscard]] core::Result<std::string> sha256OfFile(const std::filesystem::path& binary);

}
