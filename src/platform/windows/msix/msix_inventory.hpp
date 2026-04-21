#pragma once

#include <string>
#include <vector>

#include "uninstall/application.hpp"
#include "util/result.hpp"

namespace vk::platform::windows {

[[nodiscard]] core::Result<std::vector<core::Application>> enumerateMsixPackages();
[[nodiscard]] core::Result<void> removeMsixPackage(std::string_view fullName);

}
