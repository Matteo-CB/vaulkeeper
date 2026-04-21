#include "cleaners/thumbnail_cache_cleaner.hpp"

#include <filesystem>

namespace vk::core {

Result<CleanerReport> ThumbnailCacheCleaner::scan(const CleanerContext& context) {
    CleanerReport report;
    report.cleanerId = id();

    const auto dir = context.localAppData / "Microsoft/Windows/Explorer";
    if (!std::filesystem::exists(dir)) { return report; }

    std::error_code ec;
    for (auto it = std::filesystem::directory_iterator(dir, ec); it != std::filesystem::directory_iterator(); ++it) {
        if (ec) { ec.clear(); continue; }
        if (!it->is_regular_file(ec)) { continue; }
        const auto name = it->path().filename().generic_string();
        if (name.find("thumbcache") == std::string::npos && name.find("iconcache") == std::string::npos) { continue; }
        const auto size = it->file_size(ec);
        report.candidates.push_back(CleanerCandidate {
            .path = it->path(),
            .reclaimableBytes = size,
            .reason = "thumbnail or icon cache",
            .category = "explorer"
        });
        report.totalReclaimableBytes += size;
    }

    return report;
}

}
