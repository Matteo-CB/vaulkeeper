#include "uninstall/residue_scanner.hpp"

#include <algorithm>
#include <array>
#include <cctype>
#include <cstdlib>

namespace vk::core {

namespace {

std::string toLower(std::string_view in) {
    std::string out(in);
    std::transform(out.begin(), out.end(), out.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return out;
}

ResidueConfidence scoreMatch(std::string_view haystack, std::string_view needle) {
    const auto low = toLower(haystack);
    const auto low2 = toLower(needle);
    if (low2.empty()) { return ResidueConfidence::Possible; }
    if (low == low2) { return ResidueConfidence::VeryLikely; }
    if (low.find(low2) != std::string::npos) {
        if (low.size() < low2.size() * 2) { return ResidueConfidence::Likely; }
        return ResidueConfidence::Possible;
    }
    return ResidueConfidence::NeedsReview;
}

void scanDirectory(ResidueReport& report, const std::filesystem::path& root, std::string_view needle) {
    if (needle.empty() || !std::filesystem::exists(root)) { return; }
    std::error_code ec;
    for (auto it = std::filesystem::directory_iterator(root, ec); it != std::filesystem::directory_iterator(); ++it) {
        if (ec) { ec.clear(); continue; }
        const auto name = it->path().filename().generic_string();
        const auto confidence = scoreMatch(name, needle);
        if (confidence == ResidueConfidence::NeedsReview) { continue; }

        ResidueCandidate candidate;
        candidate.source = "filesystem";
        candidate.path = it->path();
        candidate.confidence = confidence;
        candidate.rationale = "name matches " + std::string(needle);

        if (it->is_regular_file(ec)) {
            candidate.sizeBytes = it->file_size(ec);
        } else if (it->is_directory(ec)) {
            for (auto inner = std::filesystem::recursive_directory_iterator(it->path(), ec);
                 inner != std::filesystem::recursive_directory_iterator(); ++inner) {
                if (ec) { ec.clear(); continue; }
                if (inner->is_regular_file(ec)) { candidate.sizeBytes += inner->file_size(ec); }
            }
        }

        report.totalBytes += candidate.sizeBytes;
        report.filesystem.push_back(std::move(candidate));
    }
}

std::filesystem::path expand(const char* env, std::string_view suffix) {
    const char* base = std::getenv(env);
    if (base == nullptr) { return {}; }
    if (suffix.empty()) { return std::filesystem::path(base); }
    return std::filesystem::path(base) / suffix;
}

}

Result<ResidueReport> scanResiduesForApplication(const std::string& applicationId, std::string_view publisher, std::string_view installLocation) {
    ResidueReport report;
    report.applicationId = applicationId;

    const std::string needle = publisher.empty() ? applicationId : std::string(publisher);
    if (!installLocation.empty()) {
        scanDirectory(report, std::filesystem::path(installLocation), needle);
    }

    const std::array<std::filesystem::path, 4> candidates {
        expand("LOCALAPPDATA", ""),
        expand("APPDATA", ""),
        expand("ProgramData", ""),
        expand("USERPROFILE", "AppData/LocalLow")
    };

    for (const auto& dir : candidates) {
        if (dir.empty()) { continue; }
        scanDirectory(report, dir, needle);
    }

    return report;
}

}
