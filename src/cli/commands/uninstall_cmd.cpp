#include "commands/uninstall_cmd.hpp"

#include <fmt/core.h>

#include "uninstall/uninstaller.hpp"

namespace vk::cli {

int UninstallCommand::run(std::span<const std::string> args, const CommandContext& ctx) {
    core::Uninstaller uninstaller(nullptr, nullptr);
    auto list = uninstaller.enumerateInstalled();
    if (!list) {
        fmt::print(stderr, "enumerate failed: {}\n", list.error().message);
        return 3;
    }
    if (args.empty() || args.front() == "list") {
        for (const auto& app : *list) {
            fmt::print("{}\t{}\t{}\t{}\n", app.id, app.displayName, app.version, app.sizeBytes);
        }
        return 0;
    }
    if (args.front() == "remove" && args.size() == 2) {
        core::UninstallPlan plan;
        plan.target.id = args[1];
        plan.runSilent = true;
        plan.sweepResidues = !ctx.dryRun;
        auto report = uninstaller.execute(plan);
        if (!report) { fmt::print(stderr, "{}\n", report.error().message); return 3; }
        fmt::print("uninstall outcome code {}\n", static_cast<int>(report->outcome));
        return 0;
    }
    fmt::print(stderr, "usage: vk uninstall [list|remove <id>]\n");
    return 2;
}

}
