#pragma once

#include "cleaners/cleaner.hpp"

namespace vk::core {

class ThumbnailCacheCleaner final : public Cleaner {
public:
    [[nodiscard]] std::string id() const override { return "system.thumbnail_cache"; }
    [[nodiscard]] std::string displayName() const override { return "Thumbnail and icon caches"; }
    [[nodiscard]] Result<CleanerReport> scan(const CleanerContext& context) override;
};

}
