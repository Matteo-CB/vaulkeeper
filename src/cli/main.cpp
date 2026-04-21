#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

#include <fmt/core.h>

#include "platform_init.hpp"
#include "commands/command.hpp"
#include "util/logger.hpp"
#include "util/version.hpp"

namespace {

void printHelp(const std::vector<vk::cli::CommandPtr>& commands) {
    fmt::print("vaulkeeper cli {}\n\n", vk::version::String);
    fmt::print("usage: vk <command> [--json|--yaml] [--verbose] [--dry-run] [args...]\n\n");
    fmt::print("commands:\n");
    for (const auto& c : commands) {
        fmt::print("  {:<14} {}\n", c->name(), c->description());
    }
    fmt::print("\nrun 'vk <command> --help' for command specific options\n");
}

}

int main(int argc, char** argv) {
    vk::platform::windows::installProviders();
    auto commands = vk::cli::registerAllCommands();
    if (argc < 2) {
        printHelp(commands);
        return 0;
    }

    std::vector<std::string> rawArgs;
    rawArgs.reserve(static_cast<std::size_t>(argc - 1));
    for (int i = 1; i < argc; ++i) {
        rawArgs.emplace_back(argv[i]);
    }

    const std::string commandName = rawArgs.front();
    rawArgs.erase(rawArgs.begin());

    if (commandName == "help" || commandName == "--help" || commandName == "-h") {
        printHelp(commands);
        return 0;
    }
    if (commandName == "--version" || commandName == "-v") {
        fmt::print("vaulkeeper {} ({} on {})\n", vk::version::String, vk::version::GitSha, vk::version::GitBranch);
        return 0;
    }

    vk::cli::CommandContext ctx;
    std::vector<std::string> filtered;
    for (auto& arg : rawArgs) {
        if (arg == "--json") { ctx.outputFormat = vk::cli::OutputFormat::Json; }
        else if (arg == "--yaml") { ctx.outputFormat = vk::cli::OutputFormat::Yaml; }
        else if (arg == "--verbose") { ctx.verbose = true; }
        else if (arg == "--dry-run") { ctx.dryRun = true; }
        else { filtered.push_back(std::move(arg)); }
    }

    auto it = std::find_if(commands.begin(), commands.end(),
                           [&](const vk::cli::CommandPtr& c) { return c->name() == commandName; });
    if (it == commands.end()) {
        fmt::print(stderr, "unknown command: {}\n", commandName);
        return 2;
    }

    return (*it)->run(std::span<const std::string> { filtered }, ctx);
}
