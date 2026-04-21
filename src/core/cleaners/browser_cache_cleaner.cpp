#include "cleaners/browser_cache_cleaner.hpp"

#include <array>
#include <filesystem>

namespace vk::core {

namespace {

struct BrowserProfile {
    std::string name;
    std::filesystem::path relativeToLocalAppData;
};

std::uint64_t sumDirectory(const std::filesystem::path& dir) {
    if (!std::filesystem::exists(dir)) { return 0; }
    std::uint64_t total = 0;
    std::error_code ec;
    for (auto it = std::filesystem::recursive_directory_iterator(dir, ec);
         it != std::filesystem::recursive_directory_iterator(); ++it) {
        if (ec) { ec.clear(); continue; }
        if (it->is_regular_file(ec)) {
            total += it->file_size(ec);
        }
    }
    return total;
}

}

Result<CleanerReport> BrowserCacheCleaner::scan(const CleanerContext& context) {
    CleanerReport report;
    report.cleanerId = id();

    const std::array<BrowserProfile, 6> browsers {{
        { "Chrome", "Google/Chrome/User Data/Default/Cache" },
        { "Edge", "Microsoft/Edge/User Data/Default/Cache" },
        { "Brave", "BraveSoftware/Brave-Browser/User Data/Default/Cache" },
        { "Opera", "Opera Software/Opera Stable/Cache" },
        { "Vivaldi", "Vivaldi/User Data/Default/Cache" },
        { "Firefox", "Mozilla/Firefox/Profiles" }
    }};

    for (const auto& b : browsers) {
        const auto path = context.localAppData / b.relativeToLocalAppData;
        const auto bytes = sumDirectory(path);
        if (bytes == 0) { continue; }
        report.totalReclaimableBytes += bytes;
        report.candidates.push_back(CleanerCandidate {
            .path = path,
            .reclaimableBytes = bytes,
            .reason = "browser cache",
            .category = b.name
        });
    }

    return report;
}

}
