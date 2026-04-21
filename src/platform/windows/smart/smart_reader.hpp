#pragma once

#include <vector>

#include "health/disk_health.hpp"
#include "util/result.hpp"

namespace vk::platform::windows {

[[nodiscard]] core::Result<std::vector<core::DiskHealthReport>> readSmartReports();

}
