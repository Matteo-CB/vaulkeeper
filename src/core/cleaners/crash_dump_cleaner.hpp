#pragma once

#include <string>

#include "cleaners/cleaner.hpp"

namespace vk::core {

class CrashDumpCleaner final : public Cleaner {
public:
    [[nodiscard]] std::string id() const override { return "system.crash_dumps"; }
    [[nodiscard]] std::string displayName() const override { return "Crash dumps and minidumps"; }
    [[nodiscard]] Result<CleanerReport> scan(const CleanerContext& context) override;
};

}
