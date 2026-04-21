#include "util/logger.hpp"

#include <filesystem>
#include <vector>

#include <spdlog/async.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace vk::core {

namespace {

spdlog::level::level_enum translate(LogLevel level) {
    switch (level) {
        case LogLevel::Trace: return spdlog::level::trace;
        case LogLevel::Debug: return spdlog::level::debug;
        case LogLevel::Info: return spdlog::level::info;
        case LogLevel::Warn: return spdlog::level::warn;
        case LogLevel::Error: return spdlog::level::err;
        case LogLevel::Critical: return spdlog::level::critical;
        case LogLevel::Off: return spdlog::level::off;
    }
    return spdlog::level::info;
}

std::shared_ptr<spdlog::logger>& root() {
    static std::shared_ptr<spdlog::logger> instance;
    return instance;
}

}

void Logger::initialize(const LogOptions& options) {
    std::filesystem::create_directories(options.directory);
    const auto filePath = (options.directory / options.fileName).string();

    std::vector<spdlog::sink_ptr> sinks;
    sinks.emplace_back(std::make_shared<spdlog::sinks::rotating_file_sink_mt>(filePath, options.maxFileBytes, options.maxFiles));
    if (options.alsoToConsole) {
        sinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
    }

    std::shared_ptr<spdlog::logger> logger;
    if (options.async) {
        spdlog::init_thread_pool(8192, 1);
        logger = std::make_shared<spdlog::async_logger>(
            "vaulkeeper", sinks.begin(), sinks.end(), spdlog::thread_pool(), spdlog::async_overflow_policy::block);
    } else {
        logger = std::make_shared<spdlog::logger>("vaulkeeper", sinks.begin(), sinks.end());
    }
    logger->set_level(translate(options.level));
    logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [%t] %v");
    logger->flush_on(spdlog::level::warn);
    spdlog::set_default_logger(logger);
    root() = logger;
}

void Logger::shutdown() noexcept {
    if (root()) {
        root()->flush();
        root().reset();
    }
    spdlog::shutdown();
}

std::shared_ptr<spdlog::logger> Logger::get() {
    return root();
}

void Logger::setLevel(LogLevel level) {
    if (root()) {
        root()->set_level(translate(level));
    }
}

}
