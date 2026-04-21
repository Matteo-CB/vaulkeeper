#include "elevated/service.hpp"

#include <array>
#include <thread>

#include <spdlog/spdlog.h>

namespace vk::agent {

ElevatedService::ElevatedService() {
    std::array<std::uint8_t, 32> key {};
    for (std::size_t i = 0; i < key.size(); ++i) {
        key[i] = static_cast<std::uint8_t>((i * 131u + 0x9Eu) & 0xFFu);
    }
    pipe = std::make_unique<core::PipeServer>("Vaulkeeper.Agent", key.data());
}

core::Result<void> ElevatedService::runConsole() {
    auto started = pipe->start([this](const core::Message& msg) { return handle(msg); });
    if (!started) { return core::fail(started.error()); }
    spdlog::info("agent console running");
    while (!stopRequested.load()) {
        std::this_thread::sleep_for(std::chrono::milliseconds { 200 });
    }
    pipe->stop();
    return core::ok();
}

core::Result<void> ElevatedService::runWindowsService() {
    return runConsole();
}

void ElevatedService::requestShutdown() {
    stopRequested.store(true);
}

core::Message ElevatedService::handle(const core::Message& msg) {
    core::Message response;
    response.header.kind = core::MessageKind::Pong;
    response.header.correlationId = msg.header.correlationId;
    response.header.version = msg.header.version;
    return response;
}

}
