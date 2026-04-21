#include "fs/volume.hpp"

#include <array>

#ifdef _WIN32
    #include <windows.h>
    #include <winioctl.h>
#endif

namespace vk::core {

namespace {

#ifdef _WIN32
FileSystemKind translateFs(const wchar_t* name) {
    if (!name) { return FileSystemKind::Unknown; }
    if (wcscmp(name, L"NTFS") == 0) return FileSystemKind::Ntfs;
    if (wcscmp(name, L"ReFS") == 0) return FileSystemKind::ReFS;
    if (wcscmp(name, L"FAT32") == 0) return FileSystemKind::Fat32;
    if (wcscmp(name, L"exFAT") == 0) return FileSystemKind::ExFat;
    if (wcscmp(name, L"CDFS") == 0) return FileSystemKind::Cdfs;
    if (wcscmp(name, L"UDF") == 0) return FileSystemKind::Udf;
    return FileSystemKind::Unknown;
}
#endif

}

Result<std::vector<Volume>> enumerateVolumes() {
#ifdef _WIN32
    std::vector<Volume> volumes;
    const auto drives = GetLogicalDrives();
    for (int i = 0; i < 26; ++i) {
        if ((drives & (1u << i)) == 0) { continue; }

        wchar_t letter[4] = { static_cast<wchar_t>(L'A' + i), L':', L'\\', 0 };
        const auto type = GetDriveTypeW(letter);
        if (type == DRIVE_NO_ROOT_DIR || type == DRIVE_UNKNOWN) { continue; }

        wchar_t fsName[MAX_PATH] {};
        wchar_t volumeLabel[MAX_PATH] {};
        DWORD serial { 0 };
        DWORD maxComponentLength { 0 };
        DWORD fsFlags { 0 };
        GetVolumeInformationW(letter, volumeLabel, MAX_PATH, &serial, &maxComponentLength, &fsFlags, fsName, MAX_PATH);

        ULARGE_INTEGER freeBytesCaller { 0 };
        ULARGE_INTEGER totalBytes { 0 };
        ULARGE_INTEGER totalFreeBytes { 0 };
        GetDiskFreeSpaceExW(letter, &freeBytesCaller, &totalBytes, &totalFreeBytes);

        Volume v;
        v.driveLetter = std::string(1, static_cast<char>('A' + i)) + ":";
        v.mountPoint = std::filesystem::path { letter };
        v.fileSystem = translateFs(fsName);
        v.totalBytes = totalBytes.QuadPart;
        v.freeBytes = totalFreeBytes.QuadPart;
        v.usedBytes = v.totalBytes > v.freeBytes ? v.totalBytes - v.freeBytes : 0;
        v.serial = serial;
        v.removable = type == DRIVE_REMOVABLE;
        v.readonly = (fsFlags & FILE_READ_ONLY_VOLUME) != 0;

        char labelUtf8[MAX_PATH] {};
        WideCharToMultiByte(CP_UTF8, 0, volumeLabel, -1, labelUtf8, sizeof(labelUtf8), nullptr, nullptr);
        v.label = labelUtf8;

        volumes.push_back(v);
    }
    return volumes;
#else
    return fail(ErrorCode::NotSupported, "volume enumeration requires Windows");
#endif
}

}
