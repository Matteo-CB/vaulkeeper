#pragma once

#include <atomic>
#include <cstddef>
#include <memory>
#include <optional>
#include <utility>

#include "util/macros.hpp"

namespace vk::core {

template <typename T>
class MpscQueue {
public:
    MpscQueue() {
        auto stub = new Node {};
        head.store(stub, std::memory_order_relaxed);
        tail = stub;
    }

    ~MpscQueue() {
        Node* node = tail;
        while (node != nullptr) {
            Node* next = node->next.load(std::memory_order_relaxed);
            delete node;
            node = next;
        }
    }

    VK_NONCOPYABLE(MpscQueue);
    VK_NONMOVABLE(MpscQueue);

    void push(T value) {
        auto node = new Node { .value = std::move(value) };
        Node* prev = head.exchange(node, std::memory_order_acq_rel);
        prev->next.store(node, std::memory_order_release);
    }

    [[nodiscard]] std::optional<T> tryPop() {
        Node* next = tail->next.load(std::memory_order_acquire);
        if (next == nullptr) {
            return std::nullopt;
        }
        T value = std::move(next->value);
        Node* old = tail;
        tail = next;
        delete old;
        return value;
    }

    [[nodiscard]] bool empty() const noexcept {
        return tail->next.load(std::memory_order_acquire) == nullptr;
    }

private:
    struct Node {
        T value {};
        std::atomic<Node*> next { nullptr };
    };

    std::atomic<Node*> head { nullptr };
    Node* tail { nullptr };
};

}
