#pragma once

#include <atomic>
#include <cstdint>
#include <functional>
#include <string>
#include <thread>

#include "ipc/message.hpp"
#include "util/macros.hpp"

namespace vk::core {

class PipeServer {
public:
    using Handler = std::function<Message(const Message&)>;

    PipeServer(std::string pipeName, std::uint8_t sharedKey[32]);
    ~PipeServer();

    VK_NONCOPYABLE(PipeServer);
    VK_NONMOVABLE(PipeServer);

    [[nodiscard]] Result<void> start(Handler handler);
    void stop();

    [[nodiscard]] bool isRunning() const noexcept;

private:
    std::string name;
    std::uint8_t key[32] {};
    Handler handler;
    std::thread acceptor;
    std::atomic<bool> running { false };

    void runLoop();
};

}
