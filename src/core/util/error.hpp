#pragma once

#include <cstdint>
#include <string>
#include <string_view>

namespace vk::core {

enum class ErrorCode : std::uint32_t {
    Ok = 0,
    Unknown,
    InvalidArgument,
    NotFound,
    PermissionDenied,
    IoError,
    NotSupported,
    AlreadyExists,
    Busy,
    Cancelled,
    OutOfMemory,
    Timeout,
    Unauthenticated,
    CorruptedData,
    VersionMismatch,
    QuotaExceeded,
    WouldDestroyData
};

struct Error {
    ErrorCode code { ErrorCode::Unknown };
    std::string message;
    std::string hint;
    std::int64_t osCode { 0 };

    [[nodiscard]] bool operator==(const Error&) const = default;
};

[[nodiscard]] Error makeError(ErrorCode code, std::string_view message);
[[nodiscard]] Error makeError(ErrorCode code, std::string_view message, std::string_view hint);
[[nodiscard]] Error fromLastOsError(std::string_view context);

[[nodiscard]] std::string_view toString(ErrorCode code) noexcept;

}
