#pragma once

#include "commands/command.hpp"

namespace vk::cli {

class HealthCommand final : public Command {
public:
    [[nodiscard]] std::string name() const override { return "health"; }
    [[nodiscard]] std::string description() const override { return "Print volume and disk health summary"; }
    [[nodiscard]] int run(std::span<const std::string> args, const CommandContext& ctx) override;
};

}
