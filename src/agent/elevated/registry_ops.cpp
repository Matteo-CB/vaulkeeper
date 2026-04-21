#include "elevated/registry_ops.hpp"

namespace vk::agent {

core::Result<std::vector<RegistryAuditEntry>> auditResidues(std::string_view) {
    return std::vector<RegistryAuditEntry> {};
}

}
