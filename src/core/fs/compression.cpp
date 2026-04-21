#include "fs/compression.hpp"

#ifdef _WIN32
    #include <windows.h>
    #include <winioctl.h>
#endif

namespace vk::core {

namespace {

#ifdef _WIN32

Result<void> applyCompression(const std::filesystem::path& path, USHORT format) {
    HANDLE handle = CreateFileW(path.c_str(), GENERIC_READ | GENERIC_WRITE,
                                FILE_SHARE_READ | FILE_SHARE_WRITE,
                                nullptr, OPEN_EXISTING,
                                FILE_FLAG_BACKUP_SEMANTICS, nullptr);
    if (handle == INVALID_HANDLE_VALUE) {
        return fromLastOsError("compression open");
    }
    DWORD returned = 0;
    const bool ok = DeviceIoControl(handle, FSCTL_SET_COMPRESSION,
                                    &format, sizeof(format),
                                    nullptr, 0, &returned, nullptr) != 0;
    CloseHandle(handle);
    if (!ok) {
        return fromLastOsError("compression set");
    }
    return ok();
}

#endif

}

Result<void> compressPathNtfs(const std::filesystem::path& path, CompressionKind kind) {
#ifdef _WIN32
    USHORT format = (kind == CompressionKind::NtfsDefault) ? COMPRESSION_FORMAT_DEFAULT : COMPRESSION_FORMAT_LZNT1;
    return applyCompression(path, format);
#else
    (void)path;
    (void)kind;
    return fail(ErrorCode::NotSupported, "compression requires Windows");
#endif
}

Result<void> decompressPathNtfs(const std::filesystem::path& path) {
#ifdef _WIN32
    return applyCompression(path, COMPRESSION_FORMAT_NONE);
#else
    (void)path;
    return fail(ErrorCode::NotSupported, "compression requires Windows");
#endif
}

Result<CompressionEstimate> estimateCompression(const std::filesystem::path& path) {
    CompressionEstimate estimate;
    std::error_code ec;
    if (!std::filesystem::is_regular_file(path, ec)) {
        return fail(ErrorCode::InvalidArgument, "compression estimate target must be a file");
    }
    estimate.originalBytes = std::filesystem::file_size(path, ec);
    if (ec) { return fail(ErrorCode::IoError, ec.message()); }
    estimate.estimatedBytes = static_cast<std::uint64_t>(static_cast<double>(estimate.originalBytes) * 0.55);
    estimate.ratio = estimate.originalBytes == 0 ? 0.0 : 1.0 - static_cast<double>(estimate.estimatedBytes) / static_cast<double>(estimate.originalBytes);
    return estimate;
}

}
