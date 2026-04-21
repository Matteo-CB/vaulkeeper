#include "cleaners/recycle_bin_cleaner.hpp"

#include <filesystem>

namespace vk::core {

namespace {

std::uint64_t inspectBin(CleanerReport& report, const std::filesystem::path& root) {
    std::uint64_t bytes = 0;
    std::error_code ec;
    if (!std::filesystem::exists(root, ec)) { return 0; }
    for (auto it = std::filesystem::recursive_directory_iterator(root, ec);
         it != std::filesystem::recursive_directory_iterator(); ++it) {
        if (ec) { ec.clear(); continue; }
        if (it->is_regular_file(ec)) {
            const auto size = it->file_size(ec);
            bytes += size;
            report.candidates.push_back(CleanerCandidate {
                .path = it->path(),
                .reclaimableBytes = size,
                .reason = "recycle bin item",
                .category = "recycle"
            });
        }
    }
    return bytes;
}

}

Result<CleanerReport> RecycleBinCleaner::scan(const CleanerContext& context) {
    CleanerReport report;
    report.cleanerId = id();
    (void)context;
    for (char letter = 'A'; letter <= 'Z'; ++letter) {
        std::filesystem::path candidate;
        candidate += letter;
        candidate += ":\\$Recycle.Bin";
        report.totalReclaimableBytes += inspectBin(report, candidate);
    }
    return report;
}

}
