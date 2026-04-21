#pragma once

#include <filesystem>

#include "util/result.hpp"

namespace vk::platform::windows {

[[nodiscard]] core::Result<std::filesystem::path> userProfile();
[[nodiscard]] core::Result<std::filesystem::path> localAppData();
[[nodiscard]] core::Result<std::filesystem::path> roamingAppData();
[[nodiscard]] core::Result<std::filesystem::path> programFiles();
[[nodiscard]] core::Result<std::filesystem::path> programFilesX86();
[[nodiscard]] core::Result<std::filesystem::path> programData();
[[nodiscard]] core::Result<std::filesystem::path> temp();

}
