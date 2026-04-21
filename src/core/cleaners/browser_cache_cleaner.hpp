#pragma once

#include "cleaners/cleaner.hpp"

namespace vk::core {

class BrowserCacheCleaner final : public Cleaner {
public:
    [[nodiscard]] std::string id() const override { return "browsers.cache"; }
    [[nodiscard]] std::string displayName() const override { return "Browser caches"; }
    [[nodiscard]] Result<CleanerReport> scan(const CleanerContext& context) override;
};

}
