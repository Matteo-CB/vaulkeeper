#pragma once

#include <string>
#include <vector>

#include "util/result.hpp"

namespace vk::platform::windows {

struct ScheduledTaskDescriptor {
    std::string path;
    std::string name;
    std::string author;
    std::string description;
    std::string action;
    std::string triggerSummary;
    bool enabled { true };
};

[[nodiscard]] core::Result<std::vector<ScheduledTaskDescriptor>> enumerateScheduledTasks();

}
