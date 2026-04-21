#pragma once

#include <cstdint>
#include <functional>
#include <string>

#include "util/result.hpp"

namespace vk::platform::windows {

struct DiskIoEvent {
    std::uint32_t processId;
    std::wstring filePath;
    std::uint64_t bytesWritten;
    std::uint64_t bytesRead;
};

class EtwSession {
public:
    using Handler = std::function<void(const DiskIoEvent&)>;

    explicit EtwSession(std::wstring sessionName);
    ~EtwSession();

    [[nodiscard]] core::Result<void> start(Handler handler);
    void stop();

private:
    std::wstring name;
    Handler handler;
    bool running { false };
};

}
