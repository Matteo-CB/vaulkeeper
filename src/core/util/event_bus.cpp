#include "util/event_bus.hpp"

#include <algorithm>

namespace vk::core {

void EventBus::unsubscribe(Subscription token) {
    if (!token.isValid()) {
        return;
    }
    std::scoped_lock lock(mutex);
    for (auto& [type, list] : listeners) {
        list.erase(std::remove_if(list.begin(), list.end(),
                                  [&](const Slot& s) { return s.id == token.id; }),
                   list.end());
    }
}

EventBus& globalEventBus() {
    static EventBus instance;
    return instance;
}

}
