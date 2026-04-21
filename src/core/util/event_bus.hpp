#pragma once

#include <cstdint>
#include <functional>
#include <mutex>
#include <typeindex>
#include <variant>
#include <vector>

#include <absl/container/flat_hash_map.h>

#include "util/macros.hpp"

namespace vk::core {

struct Subscription {
    std::uint64_t id { 0 };

    [[nodiscard]] bool isValid() const noexcept { return id != 0; }
};

class EventBus {
public:
    EventBus() = default;

    VK_NONCOPYABLE(EventBus);
    VK_NONMOVABLE(EventBus);

    template <typename Event>
    Subscription subscribe(std::function<void(const Event&)> handler) {
        std::scoped_lock lock(mutex);
        const auto id = ++nextId;
        auto& list = listeners[std::type_index(typeid(Event))];
        list.push_back(Slot {
            .id = id,
            .invoke = [h = std::move(handler)](const void* payload) {
                const auto* typed = static_cast<const Event*>(payload);
                h(*typed);
            }
        });
        return Subscription { .id = id };
    }

    template <typename Event>
    void publish(const Event& event) {
        std::vector<Slot> snapshot;
        {
            std::scoped_lock lock(mutex);
            const auto it = listeners.find(std::type_index(typeid(Event)));
            if (it == listeners.end()) {
                return;
            }
            snapshot = it->second;
        }
        for (const auto& slot : snapshot) {
            slot.invoke(&event);
        }
    }

    void unsubscribe(Subscription token);

private:
    struct Slot {
        std::uint64_t id;
        std::function<void(const void*)> invoke;
    };

    std::mutex mutex;
    std::uint64_t nextId { 0 };
    absl::flat_hash_map<std::type_index, std::vector<Slot>> listeners;
};

EventBus& globalEventBus();

}
