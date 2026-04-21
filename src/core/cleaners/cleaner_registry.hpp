#pragma once

#include <memory>
#include <vector>

#include "cleaners/cleaner.hpp"

namespace vk::core {

class CleanerRegistry {
public:
    CleanerRegistry() = default;

    void add(CleanerPtr cleaner);
    [[nodiscard]] std::vector<CleanerPtr> all() const;
    [[nodiscard]] CleanerPtr find(std::string_view id) const;

    static CleanerRegistry& defaultInstance();

private:
    std::vector<CleanerPtr> cleaners;
};

void registerBuiltInCleaners(CleanerRegistry& registry);

}
