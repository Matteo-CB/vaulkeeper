#pragma once

#include <string>

#include "cleaners/cleaner.hpp"

namespace vk::core {

class WindowsUpdateCleaner final : public Cleaner {
public:
    [[nodiscard]] std::string id() const override { return "system.windows_update"; }
    [[nodiscard]] std::string displayName() const override { return "Windows Update leftovers"; }
    [[nodiscard]] Result<CleanerReport> scan(const CleanerContext& context) override;
};

}
