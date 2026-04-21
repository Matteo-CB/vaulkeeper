#include <string>
#include <vector>

#include <spdlog/spdlog.h>

#include "config/paths.hpp"
#include "elevated/service.hpp"
#include "util/logger.hpp"

int main(int argc, char** argv) {
    auto paths = vk::core::Paths::resolveDefault(false);
    paths.ensureCreated();

    vk::core::LogOptions options;
    options.directory = paths.logDir;
    options.fileName = "agent.log";
    options.level = vk::core::LogLevel::Info;
    options.alsoToConsole = true;
    vk::core::Logger::initialize(options);

    std::vector<std::string> args;
    args.reserve(static_cast<std::size_t>(argc - 1));
    for (int i = 1; i < argc; ++i) { args.emplace_back(argv[i]); }

    vk::agent::ElevatedService service;
    const bool asService = !args.empty() && args.front() == "--service";
    auto result = asService ? service.runWindowsService() : service.runConsole();
    if (!result) {
        spdlog::critical("agent failure: {}", result.error().message);
        vk::core::Logger::shutdown();
        return 1;
    }
    vk::core::Logger::shutdown();
    return 0;
}
