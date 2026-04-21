#pragma once

#include <string>
#include <vector>

#include "util/result.hpp"

namespace vk::platform::windows {

enum class ServiceState : std::uint8_t { Stopped, Starting, Running, Stopping, Paused, Unknown };
enum class ServiceStartType : std::uint8_t { Auto, AutoDelayed, Manual, Disabled, Boot, System };

struct ServiceDescriptor {
    std::string serviceName;
    std::string displayName;
    std::string description;
    std::string binaryPath;
    std::string account;
    ServiceState state { ServiceState::Unknown };
    ServiceStartType startType { ServiceStartType::Manual };
    bool signedBinary { false };
};

[[nodiscard]] core::Result<std::vector<ServiceDescriptor>> enumerateServices();
[[nodiscard]] core::Result<void> setServiceStartType(std::string_view serviceName, ServiceStartType type);

}
