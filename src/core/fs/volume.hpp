#pragma once

#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

#include "util/bytes.hpp"
#include "util/result.hpp"

namespace vk::core {

enum class FileSystemKind : std::uint8_t {
    Unknown,
    Ntfs,
    ReFS,
    Fat32,
    ExFat,
    Cdfs,
    Udf,
    Virtual
};

struct Volume {
    std::string driveLetter;
    std::filesystem::path mountPoint;
    FileSystemKind fileSystem { FileSystemKind::Unknown };
    ByteCount totalBytes { 0 };
    ByteCount freeBytes { 0 };
    ByteCount usedBytes { 0 };
    std::string label;
    std::uint64_t serial { 0 };
    bool removable { false };
    bool readonly { false };
    bool bitlockerEnabled { false };
};

[[nodiscard]] Result<std::vector<Volume>> enumerateVolumes();

}
