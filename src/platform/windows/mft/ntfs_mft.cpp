#include "mft/ntfs_mft.hpp"

#include <array>
#include <cstring>
#include <vector>

#include <spdlog/spdlog.h>

#include "fs/path.hpp"
#include "util/string_intern.hpp"

#ifdef _WIN32
    #include <windows.h>
    #include <winioctl.h>
#endif

namespace vk::platform::windows {

namespace {

#ifdef _WIN32

constexpr std::uint32_t kAttrTypeStandardInformation = 0x10;
constexpr std::uint32_t kAttrTypeFileName = 0x30;
constexpr std::uint32_t kAttrTypeData = 0x80;
constexpr std::uint32_t kAttrTypeTerminator = 0xFFFFFFFF;

constexpr std::uint16_t kRecordFlagInUse = 0x0001;
constexpr std::uint16_t kRecordFlagIsDirectory = 0x0002;

#pragma pack(push, 1)
struct AttributeHeader {
    std::uint32_t type;
    std::uint32_t length;
    std::uint8_t nonResident;
    std::uint8_t nameLength;
    std::uint16_t nameOffset;
    std::uint16_t flags;
    std::uint16_t attributeId;
    union {
        struct {
            std::uint32_t valueLength;
            std::uint16_t valueOffset;
            std::uint8_t indexedFlag;
            std::uint8_t padding;
        } resident;
        struct {
            std::uint64_t startVcn;
            std::uint64_t endVcn;
            std::uint16_t runListOffset;
            std::uint16_t compressionUnit;
            std::uint32_t padding;
            std::uint64_t allocatedSize;
            std::uint64_t realSize;
            std::uint64_t initializedSize;
        } nonresident;
    } storage;
};

struct FileNameAttr {
    std::uint64_t parentDirectoryRef;
    std::uint64_t createdTime;
    std::uint64_t modifiedTime;
    std::uint64_t mftModifiedTime;
    std::uint64_t accessedTime;
    std::uint64_t allocatedSize;
    std::uint64_t realSize;
    std::uint32_t flags;
    std::uint32_t reparseValue;
    std::uint8_t nameLength;
    std::uint8_t nameType;
    wchar_t name[1];
};

struct StandardInformationAttr {
    std::uint64_t createdTime;
    std::uint64_t modifiedTime;
    std::uint64_t mftModifiedTime;
    std::uint64_t accessedTime;
    std::uint32_t attributes;
};
#pragma pack(pop)

HANDLE openVolume(const std::wstring& volume) {
    return CreateFileW(volume.c_str(),
                       GENERIC_READ,
                       FILE_SHARE_READ | FILE_SHARE_WRITE,
                       nullptr,
                       OPEN_EXISTING,
                       FILE_FLAG_NO_BUFFERING | FILE_FLAG_RANDOM_ACCESS,
                       nullptr);
}

bool applyUpdateSequence(std::uint8_t* record, std::uint32_t recordSize, std::uint32_t sectorSize) {
    if (record == nullptr || recordSize < 44) { return false; }
    std::uint16_t usaOffset = 0;
    std::uint16_t usaCount = 0;
    std::memcpy(&usaOffset, record + 4, sizeof(std::uint16_t));
    std::memcpy(&usaCount, record + 6, sizeof(std::uint16_t));
    if (usaCount == 0) { return false; }

    const auto sectorsCovered = usaCount - 1u;
    if (sectorsCovered * sectorSize > recordSize) { return false; }
    if (usaOffset + 2u * usaCount > recordSize) { return false; }

    std::uint16_t signature = 0;
    std::memcpy(&signature, record + usaOffset, sizeof(signature));

    for (std::uint32_t i = 0; i < sectorsCovered; ++i) {
        const std::uint32_t sectorEnd = (i + 1) * sectorSize - 2;
        std::uint16_t tail = 0;
        std::memcpy(&tail, record + sectorEnd, sizeof(tail));
        if (tail != signature) { return false; }
        std::uint16_t replacement = 0;
        std::memcpy(&replacement, record + usaOffset + 2 * (i + 1), sizeof(replacement));
        std::memcpy(record + sectorEnd, &replacement, sizeof(replacement));
    }
    return true;
}

core::TimePoint filetimeToSystemClock(std::uint64_t filetime) {
    if (filetime == 0) { return core::TimePoint {}; }
    constexpr std::uint64_t unixEpochDiff = 116444736000000000ULL;
    if (filetime < unixEpochDiff) { return core::TimePoint {}; }
    const std::uint64_t hundredNs = filetime - unixEpochDiff;
    const auto nanos = std::chrono::nanoseconds { hundredNs * 100 };
    return core::TimePoint { std::chrono::duration_cast<core::SystemClock::duration>(nanos) };
}

#endif

}

core::Result<MftBootInfo> readBootSector(const std::wstring& volumePath) {
#ifdef _WIN32
    HANDLE handle = openVolume(volumePath);
    if (handle == INVALID_HANDLE_VALUE) {
        return core::fail(core::fromLastOsError("mft open volume"));
    }

    std::array<std::uint8_t, 512> sector {};
    DWORD read = 0;
    if (!ReadFile(handle, sector.data(), static_cast<DWORD>(sector.size()), &read, nullptr)) {
        core::Error err = core::fromLastOsError("mft read boot sector");
        CloseHandle(handle);
        return std::unexpected<core::Error> { std::move(err) };
    }
    CloseHandle(handle);

    MftBootInfo info;
    std::uint16_t bytesPerSector = 0;
    std::memcpy(&bytesPerSector, sector.data() + 0x0B, sizeof(bytesPerSector));
    info.bytesPerSector = bytesPerSector;
    info.sectorsPerCluster = sector[0x0D];
    std::memcpy(&info.totalSectors, sector.data() + 0x28, sizeof(info.totalSectors));
    std::memcpy(&info.mftStartCluster, sector.data() + 0x30, sizeof(info.mftStartCluster));
    const std::int8_t clustersPerRecord = static_cast<std::int8_t>(sector[0x40]);
    if (clustersPerRecord > 0) {
        info.clustersPerMftRecord = static_cast<std::uint64_t>(clustersPerRecord);
        info.recordSizeBytes = static_cast<std::uint32_t>(clustersPerRecord * info.bytesPerSector * info.sectorsPerCluster);
    } else {
        info.recordSizeBytes = 1u << static_cast<unsigned>(-clustersPerRecord);
    }
    return info;
#else
    (void)volumePath;
    return core::fail(core::ErrorCode::NotSupported, "mft reader requires Windows");
#endif
}

NtfsMftReader::NtfsMftReader(std::wstring volumePath) : volume(std::move(volumePath)) {}

core::Result<std::size_t> NtfsMftReader::stream(const std::filesystem::path& volumeRoot,
                                                std::function<void(core::FileEntry)> sink) {
#ifdef _WIN32
    using vk::core::TimePoint;
    using vk::core::SystemClock;

    auto boot = readBootSector(volume);
    if (!boot) { return core::fail(boot.error()); }

    HANDLE handle = openVolume(volume);
    if (handle == INVALID_HANDLE_VALUE) {
        return core::fail(core::fromLastOsError("mft open"));
    }

    LARGE_INTEGER startPosition {};
    startPosition.QuadPart = static_cast<LONGLONG>(boot->mftStartCluster * boot->sectorsPerCluster * boot->bytesPerSector);
    if (!SetFilePointerEx(handle, startPosition, nullptr, FILE_BEGIN)) {
        core::Error err = core::fromLastOsError("mft seek");
        CloseHandle(handle);
        return std::unexpected<core::Error> { std::move(err) };
    }

    const std::uint32_t recordSize = boot->recordSizeBytes == 0 ? 1024u : boot->recordSizeBytes;
    constexpr std::uint32_t kRecordsPerBatch = 64;
    std::vector<std::uint8_t> buffer(static_cast<std::size_t>(recordSize) * kRecordsPerBatch);

    auto& interner = core::StringInterner::pathInterner();
    std::size_t emitted = 0;
    const std::string rootUtf8 = volumeRoot.empty() ? std::string {} : volumeRoot.generic_string();

    while (true) {
        DWORD bytesRead = 0;
        if (!ReadFile(handle, buffer.data(), static_cast<DWORD>(buffer.size()), &bytesRead, nullptr) || bytesRead == 0) {
            break;
        }
        const std::uint32_t readRecords = bytesRead / recordSize;
        for (std::uint32_t i = 0; i < readRecords; ++i) {
            std::uint8_t* record = buffer.data() + (static_cast<std::size_t>(i) * recordSize);
            if (std::memcmp(record, "FILE", 4) != 0) { continue; }
            if (!applyUpdateSequence(record, recordSize, static_cast<std::uint32_t>(boot->bytesPerSector))) { continue; }

            std::uint16_t flags = 0;
            std::memcpy(&flags, record + 22, sizeof(flags));
            if ((flags & kRecordFlagInUse) == 0) { continue; }

            std::uint16_t firstAttrOffset = 0;
            std::memcpy(&firstAttrOffset, record + 20, sizeof(firstAttrOffset));
            if (firstAttrOffset >= recordSize) { continue; }

            std::uint64_t parentRef = 0;
            std::string leafName;
            std::uint64_t realSize = 0;
            std::uint64_t allocatedSize = 0;
            std::uint64_t createdTime = 0;
            std::uint64_t modifiedTime = 0;
            std::uint64_t accessedTime = 0;
            std::uint32_t siAttributes = 0;
            bool gotFileName = false;
            bool gotStandard = false;

            std::uint32_t offset = firstAttrOffset;
            while (offset + sizeof(AttributeHeader) <= recordSize) {
                AttributeHeader header {};
                std::memcpy(&header, record + offset, sizeof(AttributeHeader));
                if (header.type == kAttrTypeTerminator) { break; }
                if (header.length == 0 || offset + header.length > recordSize) { break; }

                if (header.type == kAttrTypeStandardInformation && header.nonResident == 0) {
                    const std::uint32_t dataOffset = offset + header.storage.resident.valueOffset;
                    if (dataOffset + sizeof(StandardInformationAttr) <= recordSize) {
                        StandardInformationAttr si {};
                        std::memcpy(&si, record + dataOffset, sizeof(si));
                        createdTime = si.createdTime;
                        modifiedTime = si.modifiedTime;
                        accessedTime = si.accessedTime;
                        siAttributes = si.attributes;
                        gotStandard = true;
                    }
                } else if (header.type == kAttrTypeFileName && header.nonResident == 0) {
                    const std::uint32_t dataOffset = offset + header.storage.resident.valueOffset;
                    if (dataOffset + sizeof(FileNameAttr) <= recordSize) {
                        FileNameAttr name {};
                        std::memcpy(&name, record + dataOffset, sizeof(FileNameAttr));
                        const std::uint8_t nameLen = name.nameLength;
                        const std::uint32_t nameOffset = dataOffset + offsetof(FileNameAttr, name);
                        if (nameOffset + nameLen * 2u <= recordSize) {
                            std::wstring wide(reinterpret_cast<const wchar_t*>(record + nameOffset), nameLen);
                            const auto needed = WideCharToMultiByte(CP_UTF8, 0, wide.c_str(), nameLen, nullptr, 0, nullptr, nullptr);
                            if (needed > 0) {
                                std::string narrow(static_cast<std::size_t>(needed), '\0');
                                WideCharToMultiByte(CP_UTF8, 0, wide.c_str(), nameLen, narrow.data(), needed, nullptr, nullptr);
                                if (name.nameType != 2) {
                                    leafName = std::move(narrow);
                                    parentRef = name.parentDirectoryRef & 0x0000FFFFFFFFFFFFULL;
                                    realSize = name.realSize;
                                    allocatedSize = name.allocatedSize;
                                    gotFileName = true;
                                }
                            }
                        }
                    }
                } else if (header.type == kAttrTypeData && header.nonResident != 0 && header.nameLength == 0) {
                    if (realSize == 0) {
                        realSize = header.storage.nonresident.realSize;
                        allocatedSize = header.storage.nonresident.allocatedSize;
                    }
                }

                offset += header.length;
            }

            if (!gotFileName) { continue; }

            core::FileEntry fe;
            fe.name = interner.intern(leafName);
            fe.parentPath = interner.intern(rootUtf8);
            fe.logicalSize = realSize;
            fe.sizeOnDisk = allocatedSize == 0 ? realSize : allocatedSize;
            fe.parentRecordNumber = parentRef;
            fe.kind = (flags & kRecordFlagIsDirectory) != 0 ? core::FileKind::Directory : core::FileKind::Regular;
            if (gotStandard) {
                fe.createdAt = filetimeToSystemClock(createdTime);
                fe.modifiedAt = filetimeToSystemClock(modifiedTime);
                fe.accessedAt = filetimeToSystemClock(accessedTime);
                fe.attrs.readOnly = (siAttributes & 0x0001) != 0;
                fe.attrs.hidden = (siAttributes & 0x0002) != 0;
                fe.attrs.system = (siAttributes & 0x0004) != 0;
                fe.attrs.archive = (siAttributes & 0x0020) != 0;
                fe.attrs.compressed = (siAttributes & 0x0800) != 0;
                fe.attrs.encrypted = (siAttributes & 0x4000) != 0;
                fe.attrs.sparse = (siAttributes & 0x0200) != 0;
            }

            if (sink) { sink(std::move(fe)); }
            ++emitted;
        }

        if (readRecords < kRecordsPerBatch) { break; }
    }

    CloseHandle(handle);
    spdlog::info("mft streamed {} records from {}", emitted, volumeRoot.generic_string());
    return emitted;
#else
    (void)volumeRoot;
    (void)sink;
    return core::fail(core::ErrorCode::NotSupported, "mft requires Windows");
#endif
}

}
