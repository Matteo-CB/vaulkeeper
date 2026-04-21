#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

#include "util/result.hpp"

namespace vk::platform::windows {

enum class RegistryHive : std::uint8_t { LocalMachine, CurrentUser, ClassesRoot, Users, CurrentConfig };

struct RegistryValue {
    std::string name;
    std::string type;
    std::string stringData;
    std::uint32_t dwordData { 0 };
    std::uint64_t qwordData { 0 };
};

[[nodiscard]] core::Result<std::vector<std::string>> enumerateSubkeys(RegistryHive hive, std::string_view path, bool wow64);
[[nodiscard]] core::Result<std::vector<RegistryValue>> readValues(RegistryHive hive, std::string_view path, bool wow64);
[[nodiscard]] core::Result<void> writeString(RegistryHive hive, std::string_view path, std::string_view valueName, std::string_view data);
[[nodiscard]] core::Result<void> deleteKey(RegistryHive hive, std::string_view path, bool wow64);

}
