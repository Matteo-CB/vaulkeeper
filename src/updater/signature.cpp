#include "signature.hpp"

#include <array>
#include <fstream>

#include <fmt/format.h>

#ifdef _WIN32
    #include <softpub.h>
    #include <windows.h>
    #include <wincrypt.h>
    #include <wintrust.h>
    #pragma comment(lib, "wintrust.lib")
    #pragma comment(lib, "crypt32.lib")
#endif

namespace vk::updater {

core::Result<void> verifyAuthenticode(const std::filesystem::path& binary) {
#ifdef _WIN32
    WINTRUST_FILE_INFO fileInfo {};
    fileInfo.cbStruct = sizeof(fileInfo);
    fileInfo.pcwszFilePath = binary.c_str();

    GUID policy = WINTRUST_ACTION_GENERIC_VERIFY_V2;
    WINTRUST_DATA data {};
    data.cbStruct = sizeof(data);
    data.dwUIChoice = WTD_UI_NONE;
    data.fdwRevocationChecks = WTD_REVOKE_NONE;
    data.dwUnionChoice = WTD_CHOICE_FILE;
    data.pFile = &fileInfo;
    data.dwStateAction = WTD_STATEACTION_VERIFY;

    const auto result = WinVerifyTrust(nullptr, &policy, &data);
    data.dwStateAction = WTD_STATEACTION_CLOSE;
    WinVerifyTrust(nullptr, &policy, &data);
    if (result != 0) {
        return core::fail(core::ErrorCode::Unauthenticated, "authenticode signature invalid");
    }
    return core::ok();
#else
    (void)binary;
    return core::fail(core::ErrorCode::NotSupported, "authenticode requires Windows");
#endif
}

core::Result<std::string> sha256OfFile(const std::filesystem::path& binary) {
#ifdef _WIN32
    HCRYPTPROV provider = 0;
    if (!CryptAcquireContextW(&provider, nullptr, nullptr, PROV_RSA_AES, CRYPT_VERIFYCONTEXT)) {
        return core::fromLastOsError("crypt acquire");
    }
    HCRYPTHASH hash = 0;
    if (!CryptCreateHash(provider, CALG_SHA_256, 0, 0, &hash)) {
        CryptReleaseContext(provider, 0);
        return core::fromLastOsError("crypt create hash");
    }

    std::ifstream stream(binary, std::ios::binary);
    if (!stream) {
        CryptDestroyHash(hash);
        CryptReleaseContext(provider, 0);
        return core::fail(core::ErrorCode::IoError, "cannot open file for hashing");
    }

    std::array<std::uint8_t, 1 << 16> buffer {};
    while (stream.read(reinterpret_cast<char*>(buffer.data()), buffer.size()) || stream.gcount() > 0) {
        const auto read = static_cast<DWORD>(stream.gcount());
        if (!CryptHashData(hash, buffer.data(), read, 0)) {
            CryptDestroyHash(hash);
            CryptReleaseContext(provider, 0);
            return core::fromLastOsError("crypt hash data");
        }
    }

    std::array<std::uint8_t, 32> digest {};
    DWORD digestSize = static_cast<DWORD>(digest.size());
    if (!CryptGetHashParam(hash, HP_HASHVAL, digest.data(), &digestSize, 0)) {
        CryptDestroyHash(hash);
        CryptReleaseContext(provider, 0);
        return core::fromLastOsError("crypt get hash");
    }

    CryptDestroyHash(hash);
    CryptReleaseContext(provider, 0);

    std::string hex;
    hex.reserve(digest.size() * 2);
    for (auto byte : digest) {
        hex.append(fmt::format("{:02x}", byte));
    }
    return hex;
#else
    (void)binary;
    return core::fail(core::ErrorCode::NotSupported, "sha256 requires Windows");
#endif
}

}
