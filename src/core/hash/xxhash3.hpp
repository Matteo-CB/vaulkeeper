#pragma once

#include <cstdint>
#include <filesystem>
#include <span>
#include <string>

#include "util/result.hpp"

namespace vk::core {

struct HashDigest128 {
    std::uint64_t low { 0 };
    std::uint64_t high { 0 };

    [[nodiscard]] bool operator==(const HashDigest128&) const noexcept = default;
    [[nodiscard]] std::string toHex() const;
};

[[nodiscard]] HashDigest128 xxhash3Bytes(std::span<const std::uint8_t> data, std::uint64_t seed = 0) noexcept;
[[nodiscard]] Result<HashDigest128> xxhash3File(const std::filesystem::path& path, std::uint64_t seed = 0);
[[nodiscard]] std::uint64_t xxhash64Bytes(std::span<const std::uint8_t> data, std::uint64_t seed = 0) noexcept;

}

namespace std {
template <>
struct hash<vk::core::HashDigest128> {
    [[nodiscard]] std::size_t operator()(vk::core::HashDigest128 d) const noexcept {
        return std::hash<std::uint64_t> {}(d.low) ^ (std::hash<std::uint64_t> {}(d.high) << 1);
    }
};
}
