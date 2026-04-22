#include "plugin/plugin_host.hpp"

#include <spdlog/spdlog.h>

#ifdef _WIN32
    #include <windows.h>
    #include <wincrypt.h>
    #include <softpub.h>
    #include <wintrust.h>
    #pragma comment(lib, "wintrust")
#endif

namespace vk::core {

namespace {

#ifdef _WIN32
bool verifyAuthenticode(const std::filesystem::path& path) {
    WINTRUST_FILE_INFO fileInfo {};
    fileInfo.cbStruct = sizeof(fileInfo);
    fileInfo.pcwszFilePath = path.c_str();

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
    return result == 0;
}
#endif

}

PluginHost::~PluginHost() {
    unloadAll();
}

Result<std::vector<LoadedPlugin>> PluginHost::loadFromDirectory(const std::filesystem::path& dir) {
    if (!std::filesystem::exists(dir)) { return loadedPlugins; }

    for (const auto& entry : std::filesystem::directory_iterator(dir)) {
        if (!entry.is_regular_file() || entry.path().extension() != ".dll") { continue; }

#ifdef _WIN32
        if (!verifyAuthenticode(entry.path())) {
            spdlog::warn("plugin skipped, untrusted signature: {}", entry.path().generic_string());
            continue;
        }
        const auto handle = LoadLibraryW(entry.path().c_str());
        if (handle == nullptr) {
            spdlog::warn("plugin load failed: {}", entry.path().generic_string());
            continue;
        }
        const auto infoFn = reinterpret_cast<VkPluginInfoFn>(GetProcAddress(handle, "vk_plugin_info"));
        const auto initFn = reinterpret_cast<VkPluginInitFn>(GetProcAddress(handle, "vk_plugin_init"));
        const auto shutdownFn = reinterpret_cast<VkPluginShutdownFn>(GetProcAddress(handle, "vk_plugin_shutdown"));
        if (infoFn == nullptr || initFn == nullptr) {
            FreeLibrary(handle);
            continue;
        }
        const auto* info = infoFn();
        if (info == nullptr || info->apiVersion != vk::plugin::ApiVersion) {
            FreeLibrary(handle);
            continue;
        }
        LoadedPlugin loaded;
        loaded.name = info->name;
        loaded.path = entry.path();
        loaded.apiVersion = info->apiVersion;
        loaded.handle = handle;
        loaded.initFn = initFn;
        loaded.shutdownFn = shutdownFn;
        loadedPlugins.push_back(loaded);
#endif
    }

    return loadedPlugins;
}

void PluginHost::unloadAll() {
    for (auto& plugin : loadedPlugins) {
        if (plugin.shutdownFn != nullptr) { plugin.shutdownFn(); }
#ifdef _WIN32
        if (plugin.handle != nullptr) { FreeLibrary(static_cast<HMODULE>(plugin.handle)); }
#endif
    }
    loadedPlugins.clear();
}

}
