#pragma once

#include "commands/command.hpp"

namespace vk::cli {

class QuarantineCommand final : public Command {
public:
    [[nodiscard]] std::string name() const override { return "quarantine"; }
    [[nodiscard]] std::string description() const override { return "List, restore or purge quarantined items"; }
    [[nodiscard]] int run(std::span<const std::string> args, const CommandContext& ctx) override;
};

}
