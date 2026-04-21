#pragma once

#include <cstdint>
#include <string>

namespace vk::core {

using ByteCount = std::uint64_t;

[[nodiscard]] constexpr ByteCount operator""_KiB(unsigned long long v) noexcept {
    return static_cast<ByteCount>(v) * 1024ULL;
}

[[nodiscard]] constexpr ByteCount operator""_MiB(unsigned long long v) noexcept {
    return static_cast<ByteCount>(v) * 1024ULL * 1024ULL;
}

[[nodiscard]] constexpr ByteCount operator""_GiB(unsigned long long v) noexcept {
    return static_cast<ByteCount>(v) * 1024ULL * 1024ULL * 1024ULL;
}

[[nodiscard]] inline std::string formatBytes(ByteCount value) {
    constexpr const char* units[] = { "B", "KiB", "MiB", "GiB", "TiB", "PiB" };
    auto size = static_cast<double>(value);
    int unit = 0;
    while (size >= 1024.0 && unit < 5) {
        size /= 1024.0;
        ++unit;
    }
    char buffer[32];
    if (unit == 0) {
        std::snprintf(buffer, sizeof(buffer), "%llu %s", static_cast<unsigned long long>(value), units[0]);
    } else {
        std::snprintf(buffer, sizeof(buffer), "%.2f %s", size, units[unit]);
    }
    return std::string(buffer);
}

}
