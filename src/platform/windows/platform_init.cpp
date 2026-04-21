#include "platform_init.hpp"

#include "health/disk_health.hpp"
#include "smart/smart_reader.hpp"

namespace vk::platform::windows {

void installProviders() {
    core::setDiskHealthProvider([] {
        return readSmartReports();
    });
}

}
