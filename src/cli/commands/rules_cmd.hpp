#pragma once

#include <string>

#include "commands/command.hpp"

namespace vk::cli {

class RulesCommand final : public Command {
public:
    [[nodiscard]] std::string name() const override { return "rules"; }
    [[nodiscard]] std::string description() const override { return "Load, list or execute declarative rules"; }
    [[nodiscard]] int run(std::span<const std::string> args, const CommandContext& ctx) override;
};

}
