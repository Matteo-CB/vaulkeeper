#pragma once

#include <string>

#include "commands/command.hpp"

namespace vk::cli {

class DuplicatesCommand final : public Command {
public:
    [[nodiscard]] std::string name() const override { return "duplicates"; }
    [[nodiscard]] std::string description() const override { return "Find duplicate files under one or more folders"; }
    [[nodiscard]] int run(std::span<const std::string> args, const CommandContext& ctx) override;
};

}
