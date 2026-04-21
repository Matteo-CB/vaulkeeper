#include "health/disk_health.hpp"

#include <mutex>

namespace vk::core {

namespace {

std::mutex& providerMutex() {
    static std::mutex m;
    return m;
}

DiskHealthProvider& provider() {
    static DiskHealthProvider instance;
    return instance;
}

}

void setDiskHealthProvider(DiskHealthProvider next) {
    std::scoped_lock lock(providerMutex());
    provider() = std::move(next);
}

Result<std::vector<DiskHealthReport>> inspectPhysicalDisks() {
    DiskHealthProvider snapshot;
    {
        std::scoped_lock lock(providerMutex());
        snapshot = provider();
    }
    if (snapshot) { return snapshot(); }
    return std::vector<DiskHealthReport> {};
}

}
