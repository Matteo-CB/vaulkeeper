#pragma once

#include <string>

#include "commands/command.hpp"

namespace vk::cli {

class UninstallCommand final : public Command {
public:
    [[nodiscard]] std::string name() const override { return "uninstall"; }
    [[nodiscard]] std::string description() const override { return "List or uninstall installed applications"; }
    [[nodiscard]] int run(std::span<const std::string> args, const CommandContext& ctx) override;
};

}
