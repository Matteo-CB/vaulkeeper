#include "commands/scan_cmd.hpp"

#include <filesystem>

#include <fmt/core.h>

#include "fs/volume.hpp"
#include "scan/scan_cache.hpp"
#include "scan/scan_engine.hpp"
#include "util/bytes.hpp"
#include "util/thread_pool.hpp"

namespace vk::cli {

int ScanCommand::run(std::span<const std::string> args, const CommandContext& ctx) {
    auto pool = std::make_shared<core::ThreadPool>();
    auto cache = std::make_shared<core::ScanCache>();
    core::ScanEngine engine { pool, cache };

    core::ScanOptions options;
    if (args.empty()) {
        auto volumes = core::enumerateVolumes();
        if (!volumes) {
            fmt::print(stderr, "{}\n", volumes.error().message);
            return 3;
        }
        for (const auto& v : *volumes) {
            options.roots.push_back(v.mountPoint);
        }
    } else {
        for (const auto& a : args) {
            options.roots.emplace_back(a);
        }
    }

    auto result = engine.run(options);
    if (!result) {
        fmt::print(stderr, "scan failed: {}\n", result.error().message);
        return 3;
    }

    if (ctx.outputFormat == OutputFormat::Json) {
        fmt::print("{{\"files\":{},\"bytes\":{},\"elapsed_ns\":{}}}\n",
                   result->stats.filesVisited, result->stats.bytesIndexed, result->stats.elapsed.count());
    } else {
        fmt::print("files visited : {}\n", result->stats.filesVisited);
        fmt::print("bytes indexed : {}\n", core::formatBytes(result->stats.bytesIndexed));
        fmt::print("elapsed       : {}\n", core::formatDuration(result->stats.elapsed));
    }
    return 0;
}

}
