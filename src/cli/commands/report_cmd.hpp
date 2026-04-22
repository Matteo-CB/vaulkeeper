#pragma once

#include <string>

#include "commands/command.hpp"

namespace vk::cli {

class ReportCommand final : public Command {
public:
    [[nodiscard]] std::string name() const override { return "report"; }
    [[nodiscard]] std::string description() const override { return "Generate an audit report on stdout"; }
    [[nodiscard]] int run(std::span<const std::string> args, const CommandContext& ctx) override;
};

}
