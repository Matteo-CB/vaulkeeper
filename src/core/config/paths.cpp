#include "config/paths.hpp"

#include <cstdlib>

#ifdef _WIN32
    #include <shlobj.h>
    #include <windows.h>
#endif

namespace vk::core {

namespace {

std::filesystem::path knownFolder(int csidl) {
#ifdef _WIN32
    wchar_t buffer[MAX_PATH] {};
    if (SHGetFolderPathW(nullptr, csidl, nullptr, SHGFP_TYPE_CURRENT, buffer) == S_OK) {
        return std::filesystem::path { buffer };
    }
    return {};
#else
    return {};
#endif
}

}

Paths Paths::resolveDefault(bool portable) {
    if (portable) {
        const auto exeDir = []() -> std::filesystem::path {
#ifdef _WIN32
            wchar_t buffer[MAX_PATH] {};
            GetModuleFileNameW(nullptr, buffer, MAX_PATH);
            return std::filesystem::path { buffer }.parent_path();
#else
            return std::filesystem::current_path();
#endif
        }();
        return portableRoot(exeDir);
    }

    const auto localAppData = knownFolder(CSIDL_LOCAL_APPDATA);
    const auto appData = knownFolder(CSIDL_APPDATA);
    const auto base = localAppData.empty() ? std::filesystem::temp_directory_path() : localAppData;
    const auto configBase = appData.empty() ? base : appData;

    return Paths {
        .dataDir = base / "Vaulkeeper",
        .cacheDir = base / "Vaulkeeper" / "cache",
        .logDir = base / "Vaulkeeper" / "logs",
        .configDir = configBase / "Vaulkeeper",
        .quarantineDir = base / "Vaulkeeper" / "quarantine",
        .pluginDir = base / "Vaulkeeper" / "plugins",
        .rulesDir = configBase / "Vaulkeeper" / "rules",
        .snapshotsDir = base / "Vaulkeeper" / "snapshots"
    };
}

Paths Paths::portableRoot(const std::filesystem::path& root) {
    return Paths {
        .dataDir = root / "data",
        .cacheDir = root / "data" / "cache",
        .logDir = root / "data" / "logs",
        .configDir = root / "data" / "config",
        .quarantineDir = root / "data" / "quarantine",
        .pluginDir = root / "plugins",
        .rulesDir = root / "data" / "rules",
        .snapshotsDir = root / "data" / "snapshots"
    };
}

void Paths::ensureCreated() const {
    for (const auto& p : { dataDir, cacheDir, logDir, configDir, quarantineDir, pluginDir, rulesDir, snapshotsDir }) {
        std::error_code ec;
        std::filesystem::create_directories(p, ec);
    }
}

}
