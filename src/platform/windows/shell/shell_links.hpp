#pragma once

#include <filesystem>
#include <string>
#include <vector>

#include "util/result.hpp"

namespace vk::platform::windows {

struct Shortcut {
    std::filesystem::path linkFile;
    std::filesystem::path target;
    std::string arguments;
    std::string workingDirectory;
    bool broken { false };
};

[[nodiscard]] core::Result<std::vector<Shortcut>> enumerateShortcuts(const std::filesystem::path& directory);

}
