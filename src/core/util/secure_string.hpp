#pragma once

#include <cstddef>
#include <string>
#include <string_view>

#include "util/macros.hpp"

namespace vk::core {

class SecureString {
public:
    SecureString() = default;
    explicit SecureString(std::string_view value);
    SecureString(SecureString&& other) noexcept;
    SecureString& operator=(SecureString&& other) noexcept;
    ~SecureString();

    VK_NONCOPYABLE(SecureString);

    [[nodiscard]] std::string_view view() const noexcept;
    [[nodiscard]] std::size_t size() const noexcept;
    [[nodiscard]] bool empty() const noexcept;

    void assign(std::string_view value);
    void clear() noexcept;

private:
    std::string buffer;

    void wipe() noexcept;
};

}
