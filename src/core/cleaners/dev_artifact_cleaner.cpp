#include "cleaners/dev_artifact_cleaner.hpp"

#include <array>
#include <chrono>
#include <filesystem>

namespace vk::core {

namespace {

constexpr std::array<std::string_view, 13> Artifacts {
    "node_modules", ".venv", "__pycache__", "target", "build", "dist", "out",
    "bin", "obj", ".gradle", ".m2", ".nuget", "vendor"
};

bool isArtifactDir(std::string_view name) {
    for (auto a : Artifacts) {
        if (a == name) { return true; }
    }
    return false;
}

std::uint64_t sumDirectory(const std::filesystem::path& dir) {
    std::uint64_t total = 0;
    std::error_code ec;
    for (auto it = std::filesystem::recursive_directory_iterator(dir, ec);
         it != std::filesystem::recursive_directory_iterator(); ++it) {
        if (ec) { ec.clear(); continue; }
        if (it->is_regular_file(ec)) { total += it->file_size(ec); }
    }
    return total;
}

}

Result<CleanerReport> DevArtifactCleaner::scan(const CleanerContext& context) {
    CleanerReport report;
    report.cleanerId = id();

    const std::filesystem::path root = context.userProfile;
    if (!std::filesystem::exists(root)) { return report; }

    std::error_code ec;
    for (auto it = std::filesystem::recursive_directory_iterator(root, ec);
         it != std::filesystem::recursive_directory_iterator(); ++it) {
        if (ec) { ec.clear(); continue; }
        if (!it->is_directory(ec)) { continue; }
        const auto name = it->path().filename().generic_string();
        if (!isArtifactDir(name)) { continue; }

        const auto bytes = sumDirectory(it->path());
        if (bytes == 0) { continue; }
        report.totalReclaimableBytes += bytes;
        report.candidates.push_back(CleanerCandidate {
            .path = it->path(),
            .reclaimableBytes = bytes,
            .reason = "developer artifact",
            .category = name
        });
        it.disable_recursion_pending();
    }

    return report;
}

}
