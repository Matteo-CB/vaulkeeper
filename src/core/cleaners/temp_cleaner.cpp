#include "cleaners/temp_cleaner.hpp"

#include <array>
#include <filesystem>

namespace vk::core {

namespace {

std::uint64_t inspectDirectory(CleanerReport& report, const std::filesystem::path& dir, std::string_view category) {
    if (!std::filesystem::exists(dir)) { return 0; }
    std::uint64_t bytes = 0;
    std::error_code ec;
    for (auto it = std::filesystem::recursive_directory_iterator(dir, ec);
         it != std::filesystem::recursive_directory_iterator(); ++it) {
        if (ec) { ec.clear(); continue; }
        if (it->is_regular_file(ec)) {
            const auto size = it->file_size(ec);
            bytes += size;
            report.candidates.push_back(CleanerCandidate {
                .path = it->path(),
                .reclaimableBytes = size,
                .reason = "temporary file",
                .category = std::string(category)
            });
        }
    }
    return bytes;
}

}

Result<CleanerReport> TempCleaner::scan(const CleanerContext& context) {
    CleanerReport report;
    report.cleanerId = id();

    const std::array<std::pair<std::filesystem::path, std::string>, 3> roots {{
        { context.temp, "user_temp" },
        { context.localAppData / "Temp", "local_app_temp" },
        { std::filesystem::path { "C:/Windows/Temp" }, "windows_temp" }
    }};

    for (const auto& [path, label] : roots) {
        report.totalReclaimableBytes += inspectDirectory(report, path, label);
    }

    return report;
}

}
