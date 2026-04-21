#pragma once

#include <atomic>
#include <memory>
#include <string>

#include "ipc/pipe_server.hpp"
#include "util/result.hpp"

namespace vk::agent {

class ElevatedService {
public:
    ElevatedService();

    [[nodiscard]] core::Result<void> runConsole();
    [[nodiscard]] core::Result<void> runWindowsService();
    void requestShutdown();

private:
    std::unique_ptr<core::PipeServer> pipe;
    std::atomic<bool> stopRequested { false };

    [[nodiscard]] core::Message handle(const core::Message& msg);
};

}
