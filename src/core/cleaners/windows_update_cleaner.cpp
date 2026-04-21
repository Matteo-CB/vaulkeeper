#include "cleaners/windows_update_cleaner.hpp"

#include <array>
#include <filesystem>

namespace vk::core {

namespace {

std::uint64_t inspectDir(CleanerReport& report, const std::filesystem::path& dir, std::string_view category) {
    std::uint64_t bytes = 0;
    std::error_code ec;
    if (!std::filesystem::exists(dir, ec)) { return 0; }
    for (auto it = std::filesystem::recursive_directory_iterator(dir, ec);
         it != std::filesystem::recursive_directory_iterator(); ++it) {
        if (ec) { ec.clear(); continue; }
        if (it->is_regular_file(ec)) {
            const auto size = it->file_size(ec);
            bytes += size;
            report.candidates.push_back(CleanerCandidate {
                .path = it->path(),
                .reclaimableBytes = size,
                .reason = "windows update leftover",
                .category = std::string(category)
            });
        }
    }
    return bytes;
}

}

Result<CleanerReport> WindowsUpdateCleaner::scan(const CleanerContext& context) {
    CleanerReport report;
    report.cleanerId = id();
    (void)context;

    const std::array<std::pair<std::filesystem::path, std::string>, 4> targets {{
        { "C:/Windows/SoftwareDistribution/Download", "softwaredistribution" },
        { "C:/Windows/SoftwareDistribution/DataStore/Logs", "logs" },
        { "C:/Windows/Logs/CBS", "cbs_logs" },
        { "C:/Windows/Downloaded Program Files", "downloaded_programs" }
    }};

    for (const auto& [path, label] : targets) {
        report.totalReclaimableBytes += inspectDir(report, path, label);
    }

    return report;
}

}
