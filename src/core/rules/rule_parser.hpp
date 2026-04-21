#pragma once

#include <filesystem>
#include <vector>

#include "rules/rule.hpp"
#include "util/result.hpp"

namespace vk::core {

[[nodiscard]] Result<std::vector<Rule>> parseRulesFromYaml(const std::filesystem::path& path);
[[nodiscard]] Result<void> writeRulesToYaml(const std::filesystem::path& path, const std::vector<Rule>& rules);

}
