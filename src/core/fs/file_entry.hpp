#pragma once

#include <cstdint>
#include <vector>

#include "util/bytes.hpp"
#include "util/string_intern.hpp"
#include "util/time.hpp"

namespace vk::core {

enum class FileKind : std::uint8_t {
    Regular,
    Directory,
    SymbolicLink,
    Junction,
    HardLink,
    Offline,
    Device,
    Unknown
};

struct FileAttributes {
    bool readOnly : 1 { false };
    bool hidden : 1 { false };
    bool system : 1 { false };
    bool archive : 1 { false };
    bool compressed : 1 { false };
    bool encrypted : 1 { false };
    bool sparse : 1 { false };
    bool reparse : 1 { false };
    bool temporary : 1 { false };
    bool offline : 1 { false };
};

struct FileEntry {
    InternedString parentPath;
    InternedString name;
    ByteCount logicalSize { 0 };
    ByteCount sizeOnDisk { 0 };
    TimePoint createdAt {};
    TimePoint modifiedAt {};
    TimePoint accessedAt {};
    FileKind kind { FileKind::Unknown };
    FileAttributes attrs {};
    std::uint32_t hardLinkCount { 1 };
    std::uint64_t recordNumber { 0 };
    std::uint64_t parentRecordNumber { 0 };
    std::uint32_t usnVersion { 0 };
};

struct ScanNode {
    InternedString name;
    std::uint64_t totalFiles { 0 };
    std::uint64_t totalDirectories { 0 };
    ByteCount totalLogicalBytes { 0 };
    ByteCount totalSizeOnDisk { 0 };
    std::vector<ScanNode> children;
    std::vector<FileEntry> files;
};

}
