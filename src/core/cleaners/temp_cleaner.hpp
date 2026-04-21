#pragma once

#include "cleaners/cleaner.hpp"

namespace vk::core {

class TempCleaner final : public Cleaner {
public:
    [[nodiscard]] std::string id() const override { return "system.temp"; }
    [[nodiscard]] std::string displayName() const override { return "Temporary files"; }
    [[nodiscard]] Result<CleanerReport> scan(const CleanerContext& context) override;
};

}
