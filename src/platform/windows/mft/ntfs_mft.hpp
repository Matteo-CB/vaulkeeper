#pragma once

#include <cstdint>
#include <filesystem>
#include <functional>
#include <span>
#include <string>
#include <vector>

#include "fs/file_entry.hpp"
#include "scan/scan_engine.hpp"
#include "util/result.hpp"

namespace vk::platform::windows {

struct MftBootInfo {
    std::uint64_t bytesPerSector { 0 };
    std::uint64_t sectorsPerCluster { 0 };
    std::uint64_t clustersPerMftRecord { 0 };
    std::uint64_t mftStartCluster { 0 };
    std::uint64_t totalSectors { 0 };
    std::uint32_t recordSizeBytes { 0 };
};

struct MftRecordHeader {
    char signature[4];
    std::uint16_t usaOffset;
    std::uint16_t usaCount;
    std::uint64_t logSequenceNumber;
    std::uint16_t sequenceNumber;
    std::uint16_t hardLinkCount;
    std::uint16_t firstAttributeOffset;
    std::uint16_t flags;
    std::uint32_t usedSize;
    std::uint32_t allocatedSize;
    std::uint64_t baseRecordReference;
    std::uint16_t nextAttributeId;
    std::uint16_t align;
    std::uint32_t recordNumber;
};

[[nodiscard]] core::Result<MftBootInfo> readBootSector(const std::wstring& volumePath);

class NtfsMftReader final : public core::MftEntryProducer {
public:
    explicit NtfsMftReader(std::wstring volumePath);
    [[nodiscard]] core::Result<std::size_t> stream(const std::filesystem::path& volumeRoot,
                                                   std::function<void(core::FileEntry)> sink) override;

private:
    std::wstring volume;
};

}
