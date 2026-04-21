#include "cleaners/crash_dump_cleaner.hpp"

#include <array>
#include <filesystem>

namespace vk::core {

namespace {

std::uint64_t inspectDumps(CleanerReport& report, const std::filesystem::path& dir, std::string_view category) {
    std::uint64_t bytes = 0;
    std::error_code ec;
    if (!std::filesystem::exists(dir, ec)) { return 0; }
    for (auto it = std::filesystem::recursive_directory_iterator(dir, ec);
         it != std::filesystem::recursive_directory_iterator(); ++it) {
        if (ec) { ec.clear(); continue; }
        if (!it->is_regular_file(ec)) { continue; }
        const auto ext = it->path().extension().generic_string();
        if (ext != ".dmp" && ext != ".mdmp" && ext != ".hdmp") { continue; }
        const auto size = it->file_size(ec);
        report.candidates.push_back(CleanerCandidate {
            .path = it->path(),
            .reclaimableBytes = size,
            .reason = "crash dump",
            .category = std::string(category)
        });
        bytes += size;
    }
    return bytes;
}

}

Result<CleanerReport> CrashDumpCleaner::scan(const CleanerContext& context) {
    CleanerReport report;
    report.cleanerId = id();

    const std::array<std::pair<std::filesystem::path, std::string>, 3> roots {{
        { "C:/Windows/Minidump", "kernel_minidumps" },
        { "C:/Windows/MEMORY.DMP", "memory_dump" },
        { context.localAppData / "CrashDumps", "user_crashdumps" }
    }};

    for (const auto& [path, label] : roots) {
        std::error_code ec;
        if (std::filesystem::is_regular_file(path, ec)) {
            const auto size = std::filesystem::file_size(path, ec);
            if (!ec) {
                report.candidates.push_back(CleanerCandidate {
                    .path = path, .reclaimableBytes = size, .reason = "memory dump", .category = label
                });
                report.totalReclaimableBytes += size;
            }
        } else {
            report.totalReclaimableBytes += inspectDumps(report, path, label);
        }
    }
    return report;
}

}
