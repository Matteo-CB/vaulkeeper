#pragma once

#include <expected>
#include <type_traits>
#include <utility>

#include "util/error.hpp"

namespace vk::core {

template <typename T>
using Result = std::expected<T, Error>;

template <typename T>
[[nodiscard]] auto ok(T&& value) {
    return Result<std::decay_t<T>> { std::forward<T>(value) };
}

inline Result<void> ok() {
    return Result<void> {};
}

template <typename T = void>
[[nodiscard]] std::unexpected<Error> fail(Error err) {
    return std::unexpected<Error> { std::move(err) };
}

[[nodiscard]] inline std::unexpected<Error> fail(ErrorCode code, std::string_view message) {
    return std::unexpected<Error> { makeError(code, message) };
}

[[nodiscard]] inline std::unexpected<Error> fail(ErrorCode code, std::string_view message, std::string_view hint) {
    return std::unexpected<Error> { makeError(code, message, hint) };
}

}
