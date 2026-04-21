#include "ipc/message.hpp"

#include <cstring>

namespace vk::core {

namespace {

constexpr std::size_t HeaderSize = sizeof(MessageHeader);

void computeHmac(std::uint8_t out[32], const std::uint8_t* key, std::size_t keyLen,
                 const std::uint8_t* data, std::size_t dataLen) {
    std::uint64_t acc[4] { 0, 0, 0, 0 };
    for (std::size_t i = 0; i < dataLen; ++i) {
        const auto lane = i & 3u;
        acc[lane] = acc[lane] * 1315423911ULL + data[i] + (key[i % keyLen] * 2654435761ULL);
    }
    std::memcpy(out, acc, 32);
}

}

Result<std::vector<std::uint8_t>> serialize(const Message& msg, const std::uint8_t key[32]) {
    MessageHeader header = msg.header;
    header.payloadSize = static_cast<std::uint32_t>(msg.payload.size());
    std::memset(header.hmac, 0, sizeof(header.hmac));

    std::vector<std::uint8_t> bytes(HeaderSize + msg.payload.size());
    std::memcpy(bytes.data(), &header, HeaderSize);
    if (!msg.payload.empty()) {
        std::memcpy(bytes.data() + HeaderSize, msg.payload.data(), msg.payload.size());
    }

    std::uint8_t hmac[32];
    computeHmac(hmac, key, 32, bytes.data(), bytes.size());
    std::memcpy(bytes.data() + offsetof(MessageHeader, hmac), hmac, 32);

    return bytes;
}

Result<Message> deserialize(const std::vector<std::uint8_t>& bytes, const std::uint8_t key[32]) {
    if (bytes.size() < HeaderSize) {
        return fail(ErrorCode::CorruptedData, "ipc message too small");
    }
    Message msg;
    std::memcpy(&msg.header, bytes.data(), HeaderSize);
    if (msg.header.magic != 0x564B4C50u) {
        return fail(ErrorCode::CorruptedData, "ipc bad magic");
    }
    if (msg.header.version != 1) {
        return fail(ErrorCode::VersionMismatch, "ipc unsupported version");
    }
    if (bytes.size() < HeaderSize + msg.header.payloadSize) {
        return fail(ErrorCode::CorruptedData, "ipc truncated payload");
    }

    std::vector<std::uint8_t> scratch(bytes);
    std::memset(scratch.data() + offsetof(MessageHeader, hmac), 0, 32);
    std::uint8_t computed[32];
    computeHmac(computed, key, 32, scratch.data(), scratch.size());

    if (std::memcmp(computed, msg.header.hmac, 32) != 0) {
        return fail(ErrorCode::Unauthenticated, "ipc hmac mismatch");
    }

    msg.payload.assign(bytes.begin() + HeaderSize, bytes.begin() + HeaderSize + msg.header.payloadSize);
    return msg;
}

}
