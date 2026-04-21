#include "ipc/pipe_server.hpp"

#include <cstring>

#include <spdlog/spdlog.h>

#ifdef _WIN32
    #include <windows.h>
    #include <accctrl.h>
    #include <aclapi.h>
    #include <sddl.h>
#endif

namespace vk::core {

PipeServer::PipeServer(std::string pipeName, std::uint8_t sharedKey[32]) : name(std::move(pipeName)) {
    std::memcpy(key, sharedKey, sizeof(key));
}

PipeServer::~PipeServer() {
    stop();
}

Result<void> PipeServer::start(Handler h) {
    if (running.load()) {
        return fail(ErrorCode::AlreadyExists, "pipe server already running");
    }
    handler = std::move(h);
    running.store(true);
    acceptor = std::thread { [this] { runLoop(); } };
    return ok();
}

void PipeServer::stop() {
    if (!running.exchange(false)) {
        return;
    }
#ifdef _WIN32
    const auto fullName = std::string { "\\\\.\\pipe\\" } + name;
    HANDLE dummy = CreateFileA(fullName.c_str(), GENERIC_READ | GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, 0, nullptr);
    if (dummy != INVALID_HANDLE_VALUE) {
        CloseHandle(dummy);
    }
#endif
    if (acceptor.joinable()) {
        acceptor.join();
    }
}

bool PipeServer::isRunning() const noexcept {
    return running.load();
}

void PipeServer::runLoop() {
#ifdef _WIN32
    const auto fullName = std::string { "\\\\.\\pipe\\" } + name;

    SECURITY_ATTRIBUTES sa {};
    PSECURITY_DESCRIPTOR sd { nullptr };
    const wchar_t* sddl = L"D:(A;OICI;GRGW;;;AU)(A;OICI;GA;;;BA)(A;OICI;GA;;;SY)";
    if (!ConvertStringSecurityDescriptorToSecurityDescriptorW(sddl, SDDL_REVISION_1, &sd, nullptr)) {
        spdlog::error("pipe server failed to build DACL: {}", GetLastError());
        return;
    }
    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.lpSecurityDescriptor = sd;
    sa.bInheritHandle = FALSE;

    while (running.load()) {
        HANDLE pipe = CreateNamedPipeA(
            fullName.c_str(),
            PIPE_ACCESS_DUPLEX,
            PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
            PIPE_UNLIMITED_INSTANCES,
            64 * 1024,
            64 * 1024,
            0,
            &sa);

        if (pipe == INVALID_HANDLE_VALUE) {
            spdlog::error("CreateNamedPipe failed: {}", GetLastError());
            break;
        }

        const BOOL connected = ConnectNamedPipe(pipe, nullptr) != 0 || GetLastError() == ERROR_PIPE_CONNECTED;
        if (!connected || !running.load()) {
            CloseHandle(pipe);
            continue;
        }

        std::vector<std::uint8_t> buffer(64 * 1024);
        DWORD readBytes = 0;
        if (ReadFile(pipe, buffer.data(), static_cast<DWORD>(buffer.size()), &readBytes, nullptr) && readBytes > 0) {
            buffer.resize(readBytes);
            auto parsed = deserialize(buffer, key);
            if (parsed) {
                Message response = handler ? handler(*parsed) : Message {};
                auto bytes = serialize(response, key);
                if (bytes) {
                    DWORD written = 0;
                    WriteFile(pipe, bytes->data(), static_cast<DWORD>(bytes->size()), &written, nullptr);
                }
            } else {
                spdlog::warn("pipe server dropped message: {}", parsed.error().message);
            }
        }

        FlushFileBuffers(pipe);
        DisconnectNamedPipe(pipe);
        CloseHandle(pipe);
    }

    if (sd != nullptr) {
        LocalFree(sd);
    }
#endif
}

}
