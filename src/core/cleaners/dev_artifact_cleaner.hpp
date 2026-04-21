#pragma once

#include "cleaners/cleaner.hpp"

namespace vk::core {

class DevArtifactCleaner final : public Cleaner {
public:
    [[nodiscard]] std::string id() const override { return "dev.artifacts"; }
    [[nodiscard]] std::string displayName() const override { return "Developer build artifacts"; }
    [[nodiscard]] Result<CleanerReport> scan(const CleanerContext& context) override;
};

}
