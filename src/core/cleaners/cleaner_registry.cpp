#include "cleaners/cleaner_registry.hpp"

#include <algorithm>

#include "cleaners/browser_cache_cleaner.hpp"
#include "cleaners/crash_dump_cleaner.hpp"
#include "cleaners/dev_artifact_cleaner.hpp"
#include "cleaners/recycle_bin_cleaner.hpp"
#include "cleaners/temp_cleaner.hpp"
#include "cleaners/thumbnail_cache_cleaner.hpp"
#include "cleaners/windows_update_cleaner.hpp"

namespace vk::core {

void CleanerRegistry::add(CleanerPtr cleaner) {
    cleaners.push_back(std::move(cleaner));
}

std::vector<CleanerPtr> CleanerRegistry::all() const {
    return cleaners;
}

CleanerPtr CleanerRegistry::find(std::string_view id) const {
    auto it = std::find_if(cleaners.begin(), cleaners.end(), [&](const CleanerPtr& c) { return c->id() == id; });
    return it == cleaners.end() ? nullptr : *it;
}

CleanerRegistry& CleanerRegistry::defaultInstance() {
    static CleanerRegistry instance;
    static bool initialized = false;
    if (!initialized) {
        registerBuiltInCleaners(instance);
        initialized = true;
    }
    return instance;
}

void registerBuiltInCleaners(CleanerRegistry& registry) {
    registry.add(std::make_shared<TempCleaner>());
    registry.add(std::make_shared<BrowserCacheCleaner>());
    registry.add(std::make_shared<DevArtifactCleaner>());
    registry.add(std::make_shared<RecycleBinCleaner>());
    registry.add(std::make_shared<WindowsUpdateCleaner>());
    registry.add(std::make_shared<ThumbnailCacheCleaner>());
    registry.add(std::make_shared<CrashDumpCleaner>());
}

}
