#include "util/error.hpp"

#include <array>
#include <fmt/format.h>

#ifdef _WIN32
    #include <windows.h>
#endif

namespace vk::core {

Error makeError(ErrorCode code, std::string_view message) {
    return Error { .code = code, .message = std::string(message), .hint = {}, .osCode = 0 };
}

Error makeError(ErrorCode code, std::string_view message, std::string_view hint) {
    return Error { .code = code, .message = std::string(message), .hint = std::string(hint), .osCode = 0 };
}

Error fromLastOsError(std::string_view context) {
#ifdef _WIN32
    const auto lastError = static_cast<std::int64_t>(::GetLastError());
    wchar_t* buffer = nullptr;
    const auto formatFlags = FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS;
    const auto langId = MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT);
    const auto length = ::FormatMessageW(formatFlags, nullptr, static_cast<DWORD>(lastError), langId,
                                         reinterpret_cast<LPWSTR>(&buffer), 0, nullptr);
    std::string osMessage;
    if (length > 0 && buffer != nullptr) {
        const auto wide = std::wstring(buffer, length);
        const auto needed = ::WideCharToMultiByte(CP_UTF8, 0, wide.c_str(), static_cast<int>(wide.size()), nullptr, 0, nullptr, nullptr);
        osMessage.resize(static_cast<std::size_t>(needed));
        ::WideCharToMultiByte(CP_UTF8, 0, wide.c_str(), static_cast<int>(wide.size()), osMessage.data(), needed, nullptr, nullptr);
        ::LocalFree(buffer);
        while (!osMessage.empty() && (osMessage.back() == '\n' || osMessage.back() == '\r' || osMessage.back() == ' ')) {
            osMessage.pop_back();
        }
    }
    return Error {
        .code = ErrorCode::IoError,
        .message = fmt::format("{}: {}", context, osMessage),
        .hint = {},
        .osCode = lastError
    };
#else
    return makeError(ErrorCode::IoError, context);
#endif
}

std::string_view toString(ErrorCode code) noexcept {
    switch (code) {
        case ErrorCode::Ok: return "ok";
        case ErrorCode::Unknown: return "unknown";
        case ErrorCode::InvalidArgument: return "invalid_argument";
        case ErrorCode::NotFound: return "not_found";
        case ErrorCode::PermissionDenied: return "permission_denied";
        case ErrorCode::IoError: return "io_error";
        case ErrorCode::NotSupported: return "not_supported";
        case ErrorCode::AlreadyExists: return "already_exists";
        case ErrorCode::Busy: return "busy";
        case ErrorCode::Cancelled: return "cancelled";
        case ErrorCode::OutOfMemory: return "out_of_memory";
        case ErrorCode::Timeout: return "timeout";
        case ErrorCode::Unauthenticated: return "unauthenticated";
        case ErrorCode::CorruptedData: return "corrupted_data";
        case ErrorCode::VersionMismatch: return "version_mismatch";
        case ErrorCode::QuotaExceeded: return "quota_exceeded";
        case ErrorCode::WouldDestroyData: return "would_destroy_data";
    }
    return "unknown";
}

}
