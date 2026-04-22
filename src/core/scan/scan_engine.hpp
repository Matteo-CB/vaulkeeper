#pragma once

#include <atomic>
#include <functional>
#include <memory>
#include <filesystem>
#include <string>

#include "scan/scan_options.hpp"
#include "scan/scan_result.hpp"
#include "util/result.hpp"
#include "util/thread_pool.hpp"
#include "util/macros.hpp"

namespace vk::core {

class ScanCache;

struct ScanProgress {
    std::uint64_t filesVisited { 0 };
    std::uint64_t directoriesVisited { 0 };
    std::uint64_t bytesIndexed { 0 };
    Duration elapsed;
    double ratio { 0.0 };
    std::string currentPath;
};

using ScanProgressHandler = std::function<void(const ScanProgress&)>;

struct MftEntryProducer {
    virtual ~MftEntryProducer() = default;
    [[nodiscard]] virtual Result<std::size_t> stream(const std::filesystem::path& volumeRoot,
                                                     std::function<void(FileEntry)> sink) = 0;
};

class ScanEngine {
public:
    ScanEngine(std::shared_ptr<ThreadPool> pool, std::shared_ptr<ScanCache> cache);

    VK_NONCOPYABLE(ScanEngine);
    VK_NONMOVABLE(ScanEngine);

    void setProgressHandler(ScanProgressHandler handler);
    void setMftProducer(std::shared_ptr<MftEntryProducer> producer);

    [[nodiscard]] Result<ScanResult> run(const ScanOptions& options);
    void cancel();

private:
    std::shared_ptr<ThreadPool> pool;
    std::shared_ptr<ScanCache> cache;
    std::shared_ptr<MftEntryProducer> mft;
    ScanProgressHandler progress;
    std::atomic<bool> cancelRequested { false };

    void reportProgress(const ScanProgress& p) const;
};

}
