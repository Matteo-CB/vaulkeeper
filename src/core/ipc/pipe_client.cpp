#include "ipc/pipe_client.hpp"

#include <cstring>

#ifdef _WIN32
    #include <windows.h>
#endif

namespace vk::core {

PipeClient::PipeClient(std::string pipeName, std::uint8_t sharedKey[32]) : name(std::move(pipeName)) {
    std::memcpy(key, sharedKey, sizeof(key));
}

Result<Message> PipeClient::request(const Message& msg, std::chrono::milliseconds timeout) {
#ifdef _WIN32
    const auto fullName = std::string { "\\\\.\\pipe\\" } + name;

    if (!WaitNamedPipeA(fullName.c_str(), static_cast<DWORD>(timeout.count()))) {
        return fail(ErrorCode::Timeout, "pipe not available");
    }

    HANDLE pipe = CreateFileA(fullName.c_str(), GENERIC_READ | GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, 0, nullptr);
    if (pipe == INVALID_HANDLE_VALUE) {
        return std::unexpected<Error> { fromLastOsError("pipe open failed") };
    }

    DWORD mode = PIPE_READMODE_MESSAGE;
    SetNamedPipeHandleState(pipe, &mode, nullptr, nullptr);

    auto bytes = serialize(msg, key);
    if (!bytes) {
        CloseHandle(pipe);
        return fail(bytes.error());
    }

    DWORD written = 0;
    if (!WriteFile(pipe, bytes->data(), static_cast<DWORD>(bytes->size()), &written, nullptr)) {
        Error err = fromLastOsError("pipe write failed");
        CloseHandle(pipe);
        return std::unexpected<Error> { std::move(err) };
    }

    std::vector<std::uint8_t> buffer(64 * 1024);
    DWORD readBytes = 0;
    if (!ReadFile(pipe, buffer.data(), static_cast<DWORD>(buffer.size()), &readBytes, nullptr)) {
        Error err = fromLastOsError("pipe read failed");
        CloseHandle(pipe);
        return std::unexpected<Error> { std::move(err) };
    }
    buffer.resize(readBytes);
    CloseHandle(pipe);

    return deserialize(buffer, key);
#else
    (void)msg;
    (void)timeout;
    return fail(ErrorCode::NotSupported, "pipe client requires Windows");
#endif
}

}
