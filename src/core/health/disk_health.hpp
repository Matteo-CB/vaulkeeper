#pragma once

#include <cstdint>
#include <functional>
#include <string>
#include <vector>

#include "util/result.hpp"

namespace vk::core {

enum class DiskMediaKind : std::uint8_t { Unknown, Hdd, Ssd, Nvme, Usb };

struct SmartAttribute {
    std::uint8_t id { 0 };
    std::string name;
    std::uint64_t rawValue { 0 };
    std::uint8_t current { 0 };
    std::uint8_t worst { 0 };
    std::uint8_t threshold { 0 };
    bool critical { false };
};

struct DiskHealthReport {
    std::string deviceModel;
    std::string serial;
    std::string firmware;
    DiskMediaKind kind { DiskMediaKind::Unknown };
    std::uint64_t capacityBytes { 0 };
    std::uint64_t temperatureCelsius { 0 };
    std::uint64_t powerOnHours { 0 };
    std::uint64_t reallocatedSectors { 0 };
    std::uint64_t pendingSectors { 0 };
    std::uint64_t wearLevel { 0 };
    std::uint64_t totalHostWrites { 0 };
    std::vector<SmartAttribute> attributes;
    std::string riskAssessment;
};

using DiskHealthProvider = std::function<Result<std::vector<DiskHealthReport>>()>;

void setDiskHealthProvider(DiskHealthProvider provider);

[[nodiscard]] Result<std::vector<DiskHealthReport>> inspectPhysicalDisks();

}
