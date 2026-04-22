#pragma once

#include <string>

#include "commands/command.hpp"

namespace vk::cli {

class ScanCommand final : public Command {
public:
    [[nodiscard]] std::string name() const override { return "scan"; }
    [[nodiscard]] std::string description() const override { return "Scan volumes and print a summary"; }
    [[nodiscard]] int run(std::span<const std::string> args, const CommandContext& ctx) override;
};

}
