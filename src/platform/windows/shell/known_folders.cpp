#include "shell/known_folders.hpp"

#ifdef _WIN32
    #include <shlobj.h>
    #include <windows.h>
#endif

namespace vk::platform::windows {

namespace {

#ifdef _WIN32
core::Result<std::filesystem::path> fromKnownFolder(REFKNOWNFOLDERID id) {
    PWSTR path = nullptr;
    if (SHGetKnownFolderPath(id, 0, nullptr, &path) != S_OK) {
        return core::fail(core::fromLastOsError("known folder"));
    }
    std::filesystem::path result { path };
    CoTaskMemFree(path);
    return result;
}
#endif

}

core::Result<std::filesystem::path> userProfile() {
#ifdef _WIN32
    return fromKnownFolder(FOLDERID_Profile);
#else
    return core::fail(core::ErrorCode::NotSupported, "known folders require Windows");
#endif
}

core::Result<std::filesystem::path> localAppData() {
#ifdef _WIN32
    return fromKnownFolder(FOLDERID_LocalAppData);
#else
    return core::fail(core::ErrorCode::NotSupported, "known folders require Windows");
#endif
}

core::Result<std::filesystem::path> roamingAppData() {
#ifdef _WIN32
    return fromKnownFolder(FOLDERID_RoamingAppData);
#else
    return core::fail(core::ErrorCode::NotSupported, "known folders require Windows");
#endif
}

core::Result<std::filesystem::path> programFiles() {
#ifdef _WIN32
    return fromKnownFolder(FOLDERID_ProgramFiles);
#else
    return core::fail(core::ErrorCode::NotSupported, "known folders require Windows");
#endif
}

core::Result<std::filesystem::path> programFilesX86() {
#ifdef _WIN32
    return fromKnownFolder(FOLDERID_ProgramFilesX86);
#else
    return core::fail(core::ErrorCode::NotSupported, "known folders require Windows");
#endif
}

core::Result<std::filesystem::path> programData() {
#ifdef _WIN32
    return fromKnownFolder(FOLDERID_ProgramData);
#else
    return core::fail(core::ErrorCode::NotSupported, "known folders require Windows");
#endif
}

core::Result<std::filesystem::path> temp() {
#ifdef _WIN32
    wchar_t buffer[MAX_PATH] {};
    if (GetTempPathW(MAX_PATH, buffer) == 0) {
        return core::fail(core::fromLastOsError("temp path"));
    }
    return std::filesystem::path { buffer };
#else
    return std::filesystem::temp_directory_path();
#endif
}

}
