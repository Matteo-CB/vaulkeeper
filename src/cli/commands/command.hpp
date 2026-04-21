#pragma once

#include <memory>
#include <span>
#include <string>
#include <string_view>
#include <vector>

#include "util/result.hpp"

namespace vk::cli {

enum class OutputFormat : std::uint8_t { Text, Json, Yaml };

struct CommandContext {
    OutputFormat outputFormat { OutputFormat::Text };
    bool verbose { false };
    bool dryRun { false };
    std::filesystem::path workingDir;
};

class Command {
public:
    virtual ~Command() = default;

    [[nodiscard]] virtual std::string name() const = 0;
    [[nodiscard]] virtual std::string description() const = 0;
    [[nodiscard]] virtual int run(std::span<const std::string> args, const CommandContext& ctx) = 0;
};

using CommandPtr = std::shared_ptr<Command>;

[[nodiscard]] std::vector<CommandPtr> registerAllCommands();

}
