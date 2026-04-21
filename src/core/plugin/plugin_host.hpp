#pragma once

#include <filesystem>
#include <string>
#include <vector>

#include "plugin/plugin_api.hpp"
#include "util/result.hpp"

namespace vk::core {

struct LoadedPlugin {
    std::string name;
    std::filesystem::path path;
    std::uint32_t apiVersion { 0 };
    void* handle { nullptr };
    VkPluginInitFn initFn { nullptr };
    VkPluginShutdownFn shutdownFn { nullptr };
};

class PluginHost {
public:
    PluginHost() = default;
    ~PluginHost();

    VK_NONCOPYABLE(PluginHost);
    VK_NONMOVABLE(PluginHost);

    [[nodiscard]] Result<std::vector<LoadedPlugin>> loadFromDirectory(const std::filesystem::path& dir);
    void unloadAll();

    [[nodiscard]] std::vector<LoadedPlugin> loaded() const noexcept { return loadedPlugins; }

private:
    std::vector<LoadedPlugin> loadedPlugins;
};

}
