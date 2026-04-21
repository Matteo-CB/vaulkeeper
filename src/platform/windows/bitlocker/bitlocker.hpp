#pragma once

#include <string>
#include <vector>

#include "util/result.hpp"

namespace vk::platform::windows {

enum class BitLockerProtection : std::uint8_t { Unknown, Off, On, Suspended };

struct BitLockerStatus {
    std::string driveLetter;
    BitLockerProtection protection { BitLockerProtection::Unknown };
    std::string encryptionMethod;
    double encryptionPercentage { 0.0 };
    std::string recoveryKeyId;
};

[[nodiscard]] core::Result<std::vector<BitLockerStatus>> enumerateBitLockerVolumes();

}
