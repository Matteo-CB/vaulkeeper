#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "util/result.hpp"

namespace vk::core {

enum class MessageKind : std::uint16_t {
    Unknown = 0,
    Handshake,
    HandshakeAck,
    Ping,
    Pong,
    ScanRequest,
    ScanProgress,
    ScanResult,
    QuarantineAdd,
    QuarantineRestore,
    UninstallRequest,
    UninstallStatus,
    RuleExecute,
    ShutdownRequest,
    ErrorReport
};

struct MessageHeader {
    std::uint32_t magic { 0x564B4C50u };
    std::uint16_t version { 1 };
    MessageKind kind { MessageKind::Unknown };
    std::uint32_t payloadSize { 0 };
    std::uint64_t correlationId { 0 };
    std::uint8_t hmac[32] {};
};

struct Message {
    MessageHeader header;
    std::vector<std::uint8_t> payload;
};

[[nodiscard]] Result<std::vector<std::uint8_t>> serialize(const Message& msg, const std::uint8_t key[32]);
[[nodiscard]] Result<Message> deserialize(const std::vector<std::uint8_t>& bytes, const std::uint8_t key[32]);

}
