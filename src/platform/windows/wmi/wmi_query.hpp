#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "util/result.hpp"

namespace vk::platform::windows {

using WmiRow = std::unordered_map<std::string, std::string>;

[[nodiscard]] core::Result<std::vector<WmiRow>> queryWmi(std::wstring_view wqlQuery, std::wstring_view nameSpace = L"ROOT\\CIMV2");

}
