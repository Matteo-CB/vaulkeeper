#include "scan/scan_engine.hpp"

#include <algorithm>
#include <chrono>
#include <filesystem>
#include <type_traits>

#include <spdlog/spdlog.h>

#include "fs/path.hpp"
#include "scan/scan_cache.hpp"
#include "scan/tree_model.hpp"
#include "util/string_intern.hpp"

namespace vk::core {

ScanEngine::ScanEngine(std::shared_ptr<ThreadPool> threadPool, std::shared_ptr<ScanCache> scanCache)
    : pool(std::move(threadPool)), cache(std::move(scanCache)) {}

void ScanEngine::setProgressHandler(ScanProgressHandler handler) {
    progress = std::move(handler);
}

void ScanEngine::setMftProducer(std::shared_ptr<MftEntryProducer> producer) {
    mft = std::move(producer);
}

void ScanEngine::cancel() {
    cancelRequested.store(true);
}

void ScanEngine::reportProgress(const ScanProgress& p) const {
    if (progress) {
        progress(p);
    }
}

namespace {

FileKind kindFromEntry(const std::filesystem::directory_entry& entry, std::error_code& ec) {
    if (entry.is_symlink(ec)) { return FileKind::SymbolicLink; }
    if (entry.is_directory(ec)) { return FileKind::Directory; }
    if (entry.is_regular_file(ec)) { return FileKind::Regular; }
    return FileKind::Unknown;
}

TimePoint fsTimeToSystemClock(std::filesystem::file_time_type t) {
#if defined(__cpp_lib_chrono) && __cpp_lib_chrono >= 201907L
    if constexpr (std::is_same_v<std::filesystem::file_time_type::clock, std::chrono::file_clock>) {
        return std::chrono::clock_cast<SystemClock>(t);
    }
#endif
    const auto fsNow = std::filesystem::file_time_type::clock::now();
    const auto sysNow = SystemClock::now();
    return sysNow + std::chrono::duration_cast<SystemClock::duration>(t - fsNow);
}

Result<ScanResult> walkWithFilesystem(const ScanOptions& options, std::atomic<bool>& cancelRequested,
                                      const ScanProgressHandler& progress) {
    auto& interner = StringInterner::pathInterner();
    ScanResult result;
    result.startedAt = SystemClock::now();

    TreeBuilder builder;
    const auto steadyStart = SteadyClock::now();

    const auto iterOptions = options.followSymlinks
                                 ? std::filesystem::directory_options::follow_directory_symlink
                                 : std::filesystem::directory_options::skip_permission_denied;

    for (const auto& root : options.roots) {
        if (cancelRequested.load()) { break; }
        std::error_code ec;
        if (!std::filesystem::exists(root, ec)) {
            ++result.stats.errors;
            continue;
        }

        ScanVolumeSummary volumeSummary;
        volumeSummary.mountPoint = root;

        for (auto it = std::filesystem::recursive_directory_iterator(root, iterOptions, ec);
             it != std::filesystem::recursive_directory_iterator();
             it.increment(ec)) {
            if (cancelRequested.load()) { break; }
            if (ec) {
                ++result.stats.errors;
                ec.clear();
                continue;
            }

            const auto& entry = *it;
            FileEntry record;
            const auto parent = entry.path().parent_path().generic_string();
            const auto leafName = entry.path().filename().generic_string();
            record.parentPath = interner.intern(parent);
            record.name = interner.intern(leafName);
            record.kind = kindFromEntry(entry, ec);
            if (ec) { ec.clear(); }

            if (record.kind == FileKind::Regular) {
                record.logicalSize = entry.file_size(ec);
                if (ec) { record.logicalSize = 0; ec.clear(); }
                record.sizeOnDisk = record.logicalSize;
                ++result.stats.filesIndexed;
                result.stats.bytesIndexed += record.logicalSize;
                volumeSummary.scannedBytes += record.logicalSize;
                ++volumeSummary.fileCount;
            } else if (record.kind == FileKind::Directory) {
                ++result.stats.directoriesVisited;
                ++volumeSummary.directoryCount;
            }

            const auto mtime = entry.last_write_time(ec);
            if (!ec) { record.modifiedAt = fsTimeToSystemClock(mtime); }
            ec.clear();

            ++result.stats.filesVisited;
            builder.add(record);

            if ((result.stats.filesVisited & 0x3FFu) == 0 && progress) {
                ScanProgress sp;
                sp.filesVisited = result.stats.filesVisited;
                sp.directoriesVisited = result.stats.directoriesVisited;
                sp.bytesIndexed = result.stats.bytesIndexed;
                sp.elapsed = std::chrono::duration_cast<Duration>(SteadyClock::now() - steadyStart);
                sp.currentPath = entry.path().generic_string();
                progress(sp);
            }

            result.flatEntries.push_back(std::move(record));
        }

        volumeSummary.totalBytes = volumeSummary.scannedBytes;
        result.volumes.push_back(std::move(volumeSummary));
    }

    result.tree = builder.build();
    result.completedAt = SystemClock::now();
    result.stats.elapsed = std::chrono::duration_cast<Duration>(SteadyClock::now() - steadyStart);
    return result;
}

}

Result<ScanResult> ScanEngine::run(const ScanOptions& options) {
    cancelRequested.store(false);
    if (options.roots.empty()) {
        return fail(ErrorCode::InvalidArgument, "scan requires at least one root");
    }

    if (options.useMftFastPath && mft) {
        spdlog::info("scan engine using MFT fast path");
    }

    return walkWithFilesystem(options, cancelRequested, progress);
}

}
