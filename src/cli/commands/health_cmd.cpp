#include "commands/health_cmd.hpp"

#include <fmt/core.h>

#include "fs/volume.hpp"
#include "health/disk_health.hpp"
#include "util/bytes.hpp"

namespace vk::cli {

int HealthCommand::run(std::span<const std::string> args, const CommandContext& ctx) {
    (void)args;
    (void)ctx;

    auto volumes = core::enumerateVolumes();
    if (volumes) {
        for (const auto& v : *volumes) {
            fmt::print("{}\t{:<16}\t{}\tfree {}\t/ {}\n",
                       v.driveLetter, v.label, v.mountPoint.generic_string(),
                       core::formatBytes(v.freeBytes), core::formatBytes(v.totalBytes));
        }
    }
    auto disks = core::inspectPhysicalDisks();
    if (disks) {
        for (const auto& d : *disks) {
            fmt::print("disk\t{}\t{} hours\tcapacity {}\trisk {}\n",
                       d.deviceModel, d.powerOnHours, core::formatBytes(d.capacityBytes), d.riskAssessment);
        }
    }
    return 0;
}

}
