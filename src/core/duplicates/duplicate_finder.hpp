#pragma once

#include <cstdint>
#include <filesystem>
#include <functional>
#include <vector>

#include "hash/xxhash3.hpp"
#include "util/bytes.hpp"
#include "util/result.hpp"

namespace vk::core {

struct DuplicateFile {
    std::filesystem::path path;
    ByteCount size { 0 };
    HashDigest128 contentDigest {};
};

struct DuplicateCluster {
    ByteCount fileSize { 0 };
    HashDigest128 digest {};
    std::vector<DuplicateFile> files;
    [[nodiscard]] ByteCount recoverableBytes() const noexcept {
        if (files.size() <= 1) { return 0; }
        return fileSize * (files.size() - 1);
    }
};

struct DuplicateOptions {
    bool includeZeroByte { false };
    std::uint64_t minSize { 4 * 1024 };
    std::size_t hashWorkers { 0 };
    std::function<void(std::size_t inspected, std::size_t total)> progress;
};

[[nodiscard]] Result<std::vector<DuplicateCluster>> findDuplicates(const std::vector<std::filesystem::path>& candidates, const DuplicateOptions& options);

}
