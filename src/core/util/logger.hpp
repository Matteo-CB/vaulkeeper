#pragma once

#include <filesystem>
#include <memory>
#include <string_view>

#include <spdlog/spdlog.h>
#include <string>

#include "util/macros.hpp"

namespace vk::core {

enum class LogLevel { Trace, Debug, Info, Warn, Error, Critical, Off };

struct LogOptions {
    std::filesystem::path directory;
    std::string fileName { "vaulkeeper.log" };
    std::size_t maxFileBytes { 16ULL * 1024ULL * 1024ULL };
    std::size_t maxFiles { 5 };
    LogLevel level { LogLevel::Info };
    bool alsoToConsole { true };
    bool async { true };
};

class Logger {
public:
    static void initialize(const LogOptions& options);
    static void shutdown() noexcept;

    static std::shared_ptr<spdlog::logger> get();
    static void setLevel(LogLevel level);

    VK_NONCOPYABLE(Logger);
    VK_NONMOVABLE(Logger);

private:
    Logger() = delete;
};

}
