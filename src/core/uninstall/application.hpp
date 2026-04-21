#pragma once

#include <chrono>
#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

namespace vk::core {

enum class InstallKind : std::uint8_t {
    Unknown,
    Msi,
    Msix,
    Win32Classic,
    Steam,
    EpicGames,
    Gog,
    XboxGamePass,
    EaApp,
    UbisoftConnect,
    Portable,
    Winget,
    Scoop,
    Chocolatey
};

struct UninstalledComponent {
    std::string name;
    std::filesystem::path path;
    std::uint64_t sizeBytes { 0 };
    std::string kind;
};

struct Application {
    std::string id;
    std::string displayName;
    std::string publisher;
    std::string version;
    InstallKind installKind { InstallKind::Unknown };
    std::filesystem::path installLocation;
    std::string uninstallString;
    std::string silentUninstallString;
    std::chrono::system_clock::time_point installedAt;
    std::chrono::system_clock::time_point lastUsedAt;
    std::uint64_t sizeBytes { 0 };
    bool systemComponent { false };
    std::string bloatwareScore;
    std::vector<UninstalledComponent> knownComponents;
};

}
