#include "util/string_intern.hpp"

namespace vk::core {

StringInterner::StringInterner() {
    storage.emplace_back();
    index.reserve(1 << 14);
    storage.reserve(1 << 14);
}

InternedString StringInterner::intern(std::string_view value) {
    std::scoped_lock lock(mutex);
    if (auto it = index.find(std::string(value)); it != index.end()) {
        return InternedString { .id = it->second };
    }
    const auto id = static_cast<std::uint32_t>(storage.size());
    storage.emplace_back(value);
    bytes += value.size();
    index.emplace(storage.back(), id);
    return InternedString { .id = id };
}

std::string_view StringInterner::view(InternedString handle) const {
    std::scoped_lock lock(mutex);
    if (handle.id >= storage.size()) {
        return {};
    }
    return storage[handle.id];
}

std::size_t StringInterner::size() const noexcept {
    std::scoped_lock lock(mutex);
    return storage.empty() ? 0 : storage.size() - 1;
}

std::size_t StringInterner::memoryFootprint() const noexcept {
    return bytes;
}

StringInterner& StringInterner::pathInterner() {
    static StringInterner instance;
    return instance;
}

}
