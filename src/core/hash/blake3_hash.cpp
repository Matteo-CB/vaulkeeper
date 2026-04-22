#include "hash/blake3_hash.hpp"

#include <cstdio>

#include <blake3.h>

#ifdef _WIN32
    #include <windows.h>
#endif

namespace vk::core {

Blake3Digest blake3Bytes(std::span<const std::uint8_t> data) noexcept {
    blake3_hasher hasher;
    blake3_hasher_init(&hasher);
    blake3_hasher_update(&hasher, data.data(), data.size());
    Blake3Digest digest {};
    blake3_hasher_finalize(&hasher, digest.data(), digest.size());
    return digest;
}

Result<Blake3Digest> blake3File(const std::filesystem::path& path) {
#ifdef _WIN32
    HANDLE file = CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                              nullptr, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, nullptr);
    if (file == INVALID_HANDLE_VALUE) {
        return std::unexpected<Error> { fromLastOsError("blake3 open failed") };
    }

    blake3_hasher hasher;
    blake3_hasher_init(&hasher);

    std::array<std::uint8_t, 1 << 16> buffer {};
    DWORD read = 0;
    while (ReadFile(file, buffer.data(), static_cast<DWORD>(buffer.size()), &read, nullptr) && read > 0) {
        blake3_hasher_update(&hasher, buffer.data(), read);
    }
    CloseHandle(file);

    Blake3Digest digest {};
    blake3_hasher_finalize(&hasher, digest.data(), digest.size());
    return digest;
#else
    (void)path;
    return fail(ErrorCode::NotSupported, "blake3 file requires Windows");
#endif
}

std::string digestToHex(const Blake3Digest& digest) {
    std::string out(digest.size() * 2, '0');
    for (std::size_t i = 0; i < digest.size(); ++i) {
        std::snprintf(out.data() + (i * 2), 3, "%02x", digest[i]);
    }
    return out;
}

}
