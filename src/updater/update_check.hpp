#pragma once

#include <string>

#include "util/result.hpp"

namespace vk::updater {

struct UpdateManifest {
    std::string latestVersion;
    std::string downloadUrl;
    std::string sha256;
    std::string releaseNotes;
};

[[nodiscard]] core::Result<UpdateManifest> fetchManifest(std::string_view endpoint);
[[nodiscard]] bool newerThanCurrent(std::string_view candidate, std::string_view current);

}
