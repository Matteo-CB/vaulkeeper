#include "commands/quarantine_cmd.hpp"

#include <fmt/core.h>

#include "config/paths.hpp"
#include "quarantine/quarantine.hpp"

namespace vk::cli {

int QuarantineCommand::run(std::span<const std::string> args, const CommandContext& ctx) {
    const auto paths = core::Paths::resolveDefault(false);
    core::Quarantine store;
    auto init = store.initialize(paths.quarantineDir, std::chrono::hours { 24 * 30 });
    if (!init) {
        fmt::print(stderr, "quarantine init failed: {}\n", init.error().message);
        return 3;
    }

    if (args.empty() || args.front() == "list") {
        auto list = store.list();
        if (!list) { return 3; }
        for (const auto& item : *list) {
            fmt::print("{}\t{}\t{} B\n", item.id, item.originalPath.generic_string(), item.storedSizeBytes);
        }
        fmt::print("total stored : {} B\n", store.totalStoredBytes());
        return 0;
    }
    if (args.front() == "restore" && args.size() == 2) {
        auto result = store.restore(args[1]);
        if (!result) { fmt::print(stderr, "{}\n", result.error().message); return 3; }
        return 0;
    }
    if (args.front() == "discard" && args.size() == 2) {
        auto result = store.discard(args[1]);
        if (!result) { fmt::print(stderr, "{}\n", result.error().message); return 3; }
        return 0;
    }
    if (args.front() == "purge") {
        auto result = store.purgeExpired();
        if (!result) { fmt::print(stderr, "{}\n", result.error().message); return 3; }
        fmt::print("purged {} items\n", *result);
        return 0;
    }

    fmt::print(stderr, "usage: vk quarantine [list|restore <id>|discard <id>|purge]\n");
    (void)ctx;
    return 2;
}

}
