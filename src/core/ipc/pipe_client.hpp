#pragma once

#include <chrono>
#include <cstdint>
#include <string>

#include "ipc/message.hpp"
#include "util/macros.hpp"

namespace vk::core {

class PipeClient {
public:
    PipeClient(std::string pipeName, std::uint8_t sharedKey[32]);

    VK_NONCOPYABLE(PipeClient);
    VK_NONMOVABLE(PipeClient);

    [[nodiscard]] Result<Message> request(const Message& msg, std::chrono::milliseconds timeout = std::chrono::seconds { 5 });

private:
    std::string name;
    std::uint8_t key[32] {};
};

}
