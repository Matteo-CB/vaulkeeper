#include "commands/duplicates_cmd.hpp"

#include <filesystem>

#include <fmt/core.h>

#include "duplicates/duplicate_finder.hpp"
#include "util/bytes.hpp"

namespace vk::cli {

int DuplicatesCommand::run(std::span<const std::string> args, const CommandContext& ctx) {
    if (args.empty()) {
        fmt::print(stderr, "duplicates requires at least one folder\n");
        return 2;
    }

    std::vector<std::filesystem::path> candidates;
    std::error_code ec;
    for (const auto& root : args) {
        for (auto it = std::filesystem::recursive_directory_iterator(root, ec);
             it != std::filesystem::recursive_directory_iterator(); ++it) {
            if (ec) { ec.clear(); continue; }
            if (it->is_regular_file(ec)) { candidates.push_back(it->path()); }
        }
    }

    core::DuplicateOptions options;
    auto clusters = core::findDuplicates(candidates, options);
    if (!clusters) {
        fmt::print(stderr, "duplicates failed: {}\n", clusters.error().message);
        return 3;
    }

    std::uint64_t totalRecoverable = 0;
    for (const auto& c : *clusters) { totalRecoverable += c.recoverableBytes(); }

    if (ctx.outputFormat == OutputFormat::Json) {
        fmt::print("{{\"clusters\":{},\"recoverable_bytes\":{}}}\n", clusters->size(), totalRecoverable);
    } else {
        fmt::print("clusters    : {}\n", clusters->size());
        fmt::print("recoverable : {}\n", core::formatBytes(totalRecoverable));
    }
    return 0;
}

}
