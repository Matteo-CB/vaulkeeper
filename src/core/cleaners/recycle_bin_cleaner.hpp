#pragma once

#include <string>

#include "cleaners/cleaner.hpp"

namespace vk::core {

class RecycleBinCleaner final : public Cleaner {
public:
    [[nodiscard]] std::string id() const override { return "system.recycle_bin"; }
    [[nodiscard]] std::string displayName() const override { return "Recycle bin"; }
    [[nodiscard]] Result<CleanerReport> scan(const CleanerContext& context) override;
};

}
