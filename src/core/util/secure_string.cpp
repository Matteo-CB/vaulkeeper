#include "util/secure_string.hpp"

#ifdef _WIN32
    #include <windows.h>
#else
    #include <cstring>
#endif

namespace vk::core {

SecureString::SecureString(std::string_view value) {
    assign(value);
}

SecureString::SecureString(SecureString&& other) noexcept : buffer(std::move(other.buffer)) {
    other.wipe();
}

SecureString& SecureString::operator=(SecureString&& other) noexcept {
    if (this != &other) {
        wipe();
        buffer = std::move(other.buffer);
        other.wipe();
    }
    return *this;
}

SecureString::~SecureString() {
    wipe();
}

std::string_view SecureString::view() const noexcept {
    return buffer;
}

std::size_t SecureString::size() const noexcept {
    return buffer.size();
}

bool SecureString::empty() const noexcept {
    return buffer.empty();
}

void SecureString::assign(std::string_view value) {
    wipe();
    buffer.assign(value.data(), value.size());
}

void SecureString::clear() noexcept {
    wipe();
}

void SecureString::wipe() noexcept {
    if (buffer.empty()) {
        return;
    }
#ifdef _WIN32
    SecureZeroMemory(buffer.data(), buffer.size());
#else
    std::memset(buffer.data(), 0, buffer.size());
#endif
    buffer.clear();
    buffer.shrink_to_fit();
}

}
