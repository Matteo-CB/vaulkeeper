#include <array>

#include <catch2/catch_all.hpp>

#include "ipc/message.hpp"

using namespace vk::core;

TEST_CASE("ipc serialize round trip", "[ipc]") {
    std::array<std::uint8_t, 32> key {};
    for (std::size_t i = 0; i < key.size(); ++i) { key[i] = static_cast<std::uint8_t>(i); }

    Message msg;
    msg.header.kind = MessageKind::Ping;
    msg.header.correlationId = 0xDEADBEEF;
    msg.payload = { 'h', 'i' };

    auto encoded = serialize(msg, key.data());
    REQUIRE(encoded.has_value());
    auto decoded = deserialize(*encoded, key.data());
    REQUIRE(decoded.has_value());
    REQUIRE(decoded->header.kind == MessageKind::Ping);
    REQUIRE(decoded->payload.size() == 2);
}

TEST_CASE("ipc rejects tampered payload", "[ipc]") {
    std::array<std::uint8_t, 32> key {};
    key[0] = 1;

    Message msg;
    msg.header.kind = MessageKind::Ping;
    msg.payload = { 1, 2, 3 };
    auto encoded = serialize(msg, key.data());
    REQUIRE(encoded.has_value());
    (*encoded).back() ^= 0xFF;
    auto decoded = deserialize(*encoded, key.data());
    REQUIRE_FALSE(decoded.has_value());
}
