#include "hash/xxhash3.hpp"

#include <array>
#include <cstdio>

#define XXH_INLINE_ALL
#include <xxhash.h>

#ifdef _WIN32
    #include <windows.h>
#endif

namespace vk::core {

std::string HashDigest128::toHex() const {
    char buffer[33] {};
    std::snprintf(buffer, sizeof(buffer), "%016llx%016llx",
                  static_cast<unsigned long long>(high),
                  static_cast<unsigned long long>(low));
    return std::string(buffer);
}

HashDigest128 xxhash3Bytes(std::span<const std::uint8_t> data, std::uint64_t seed) noexcept {
    const auto digest = XXH3_128bits_withSeed(data.data(), data.size(), seed);
    return HashDigest128 { .low = digest.low64, .high = digest.high64 };
}

std::uint64_t xxhash64Bytes(std::span<const std::uint8_t> data, std::uint64_t seed) noexcept {
    return XXH3_64bits_withSeed(data.data(), data.size(), seed);
}

Result<HashDigest128> xxhash3File(const std::filesystem::path& path, std::uint64_t seed) {
#ifdef _WIN32
    HANDLE file = CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                              nullptr, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, nullptr);
    if (file == INVALID_HANDLE_VALUE) {
        return std::unexpected<Error> { fromLastOsError("hash open failed") };
    }

    LARGE_INTEGER size;
    if (!GetFileSizeEx(file, &size)) {
        Error err = fromLastOsError("file size query failed");
        CloseHandle(file);
        return std::unexpected<Error> { std::move(err) };
    }

    XXH3_state_t* state = XXH3_createState();
    XXH3_128bits_reset_withSeed(state, seed);

    std::array<std::uint8_t, 1 << 16> buffer {};
    DWORD read = 0;
    while (ReadFile(file, buffer.data(), static_cast<DWORD>(buffer.size()), &read, nullptr) && read > 0) {
        XXH3_128bits_update(state, buffer.data(), read);
    }
    CloseHandle(file);

    const auto digest = XXH3_128bits_digest(state);
    XXH3_freeState(state);
    return HashDigest128 { .low = digest.low64, .high = digest.high64 };
#else
    (void)path;
    (void)seed;
    return fail(ErrorCode::NotSupported, "hash file requires Windows");
#endif
}

}
