#pragma once

#include <cstdint>

namespace vk::plugin {

struct PluginInfo {
    const char* name;
    const char* vendor;
    std::uint32_t versionMajor;
    std::uint32_t versionMinor;
    std::uint32_t versionPatch;
    std::uint32_t apiVersion;
};

struct PluginContext {
    void* hostInterface;
    std::uint32_t hostApiVersion;
    const char* userDataDir;
};

constexpr std::uint32_t ApiVersion = 1;

}

extern "C" {
    using VkPluginInfoFn = const vk::plugin::PluginInfo* (*)();
    using VkPluginInitFn = int (*)(const vk::plugin::PluginContext*);
    using VkPluginShutdownFn = void (*)();
}
