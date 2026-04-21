#pragma once

#include <filesystem>

namespace vk::core {

struct Paths {
    std::filesystem::path dataDir;
    std::filesystem::path cacheDir;
    std::filesystem::path logDir;
    std::filesystem::path configDir;
    std::filesystem::path quarantineDir;
    std::filesystem::path pluginDir;
    std::filesystem::path rulesDir;
    std::filesystem::path snapshotsDir;

    [[nodiscard]] static Paths resolveDefault(bool portable);
    [[nodiscard]] static Paths portableRoot(const std::filesystem::path& root);
    void ensureCreated() const;
};

}
