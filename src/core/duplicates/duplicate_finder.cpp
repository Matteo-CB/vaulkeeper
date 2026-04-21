#include "duplicates/duplicate_finder.hpp"

#include <algorithm>
#include <mutex>

#include <absl/container/flat_hash_map.h>

namespace vk::core {

Result<std::vector<DuplicateCluster>> findDuplicates(const std::vector<std::filesystem::path>& candidates, const DuplicateOptions& options) {
    absl::flat_hash_map<ByteCount, std::vector<std::filesystem::path>> bySize;
    for (const auto& path : candidates) {
        std::error_code ec;
        const auto size = std::filesystem::file_size(path, ec);
        if (ec) { continue; }
        if (size == 0 && !options.includeZeroByte) { continue; }
        if (size < options.minSize) { continue; }
        bySize[size].push_back(path);
    }

    std::vector<DuplicateCluster> clusters;
    std::size_t inspected = 0;
    const auto total = candidates.size();
    for (auto& [size, paths] : bySize) {
        if (paths.size() < 2) { inspected += paths.size(); continue; }

        absl::flat_hash_map<HashDigest128, std::vector<std::filesystem::path>> byDigest;
        for (const auto& p : paths) {
            auto digest = xxhash3File(p);
            if (!digest) { ++inspected; continue; }
            byDigest[*digest].push_back(p);
            ++inspected;
            if (options.progress) { options.progress(inspected, total); }
        }

        for (auto& [digest, groupPaths] : byDigest) {
            if (groupPaths.size() < 2) { continue; }
            DuplicateCluster cluster;
            cluster.digest = digest;
            cluster.fileSize = size;
            cluster.files.reserve(groupPaths.size());
            for (auto& g : groupPaths) {
                cluster.files.push_back(DuplicateFile { .path = std::move(g), .size = size, .contentDigest = digest });
            }
            clusters.push_back(std::move(cluster));
        }
    }

    std::sort(clusters.begin(), clusters.end(),
              [](const DuplicateCluster& a, const DuplicateCluster& b) { return a.recoverableBytes() > b.recoverableBytes(); });
    return clusters;
}

}
