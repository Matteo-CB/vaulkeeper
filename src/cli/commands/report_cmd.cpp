#include "commands/report_cmd.hpp"

#include <fmt/core.h>

#include "util/time.hpp"
#include "util/version.hpp"

namespace vk::cli {

int ReportCommand::run(std::span<const std::string> args, const CommandContext& ctx) {
    (void)args;
    fmt::print("# Vaulkeeper audit\n");
    fmt::print("version: {}\n", vk::version::String);
    fmt::print("generated: {}\n", core::formatIso8601Utc(core::SystemClock::now()));
    fmt::print("format: {}\n", ctx.outputFormat == OutputFormat::Json ? "json" : "text");
    return 0;
}

}
