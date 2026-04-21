#pragma once

#include <string>
#include <vector>

#include "util/result.hpp"

namespace vk::agent {

struct RegistryAuditEntry {
    std::string hive;
    std::string path;
    std::string action;
};

[[nodiscard]] core::Result<std::vector<RegistryAuditEntry>> auditResidues(std::string_view applicationId);

}
