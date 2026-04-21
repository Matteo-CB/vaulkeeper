#pragma once

#include <cstdint>
#include <mutex>
#include <string>
#include <string_view>
#include <vector>

#include <absl/container/flat_hash_map.h>

#include "util/macros.hpp"

namespace vk::core {

struct InternedString {
    std::uint32_t id { 0 };

    [[nodiscard]] constexpr bool isValid() const noexcept { return id != 0; }
    [[nodiscard]] constexpr bool operator==(const InternedString&) const noexcept = default;
};

class StringInterner {
public:
    StringInterner();

    VK_NONCOPYABLE(StringInterner);
    VK_NONMOVABLE(StringInterner);

    [[nodiscard]] InternedString intern(std::string_view value);
    [[nodiscard]] std::string_view view(InternedString handle) const;
    [[nodiscard]] std::size_t size() const noexcept;
    [[nodiscard]] std::size_t memoryFootprint() const noexcept;

    static StringInterner& pathInterner();

private:
    mutable std::mutex mutex;
    absl::flat_hash_map<std::string, std::uint32_t> index;
    std::vector<std::string> storage;
    std::size_t bytes { 0 };
};

}

namespace std {
template <>
struct hash<vk::core::InternedString> {
    [[nodiscard]] std::size_t operator()(vk::core::InternedString value) const noexcept {
        return std::hash<std::uint32_t> {}(value.id);
    }
};
}
