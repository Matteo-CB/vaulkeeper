#include "fs/secure_delete.hpp"

#include <array>
#include <fstream>
#include <random>
#include <vector>

namespace vk::core {

namespace {

std::uint32_t passesFor(SecureDeleteStandard standard) {
    switch (standard) {
        case SecureDeleteStandard::Zero: return 1;
        case SecureDeleteStandard::Dod522022M: return 3;
        case SecureDeleteStandard::Gutmann: return 35;
    }
    return 1;
}

void fillPattern(std::vector<std::uint8_t>& buffer, std::uint32_t passIndex, SecureDeleteStandard standard) {
    if (standard == SecureDeleteStandard::Zero) {
        std::fill(buffer.begin(), buffer.end(), 0u);
        return;
    }
    if (standard == SecureDeleteStandard::Dod522022M) {
        if (passIndex == 0) { std::fill(buffer.begin(), buffer.end(), 0u); return; }
        if (passIndex == 1) { std::fill(buffer.begin(), buffer.end(), 0xFFu); return; }
    }
    static thread_local std::mt19937 rng { std::random_device {}() };
    std::uniform_int_distribution<int> dist { 0, 255 };
    for (auto& byte : buffer) { byte = static_cast<std::uint8_t>(dist(rng)); }
}

}

Result<std::uint64_t> secureDeleteFile(const std::filesystem::path& path, const SecureDeleteOptions& options) {
    std::error_code ec;
    if (!std::filesystem::is_regular_file(path, ec)) {
        return fail(ErrorCode::InvalidArgument, "secure delete target must be a regular file");
    }

    const auto size = std::filesystem::file_size(path, ec);
    if (ec) { return fail(ErrorCode::IoError, ec.message()); }

    std::fstream file(path, std::ios::in | std::ios::out | std::ios::binary);
    if (!file) {
        return fail(ErrorCode::PermissionDenied, "secure delete cannot open file");
    }

    const auto passes = passesFor(options.standard);
    std::vector<std::uint8_t> buffer(options.bufferSize, 0u);

    for (std::uint32_t pass = 0; pass < passes; ++pass) {
        fillPattern(buffer, pass, options.standard);
        file.seekp(0);
        std::uint64_t remaining = size;
        while (remaining > 0) {
            const auto chunk = static_cast<std::uint64_t>(std::min<std::uint64_t>(remaining, buffer.size()));
            file.write(reinterpret_cast<const char*>(buffer.data()), static_cast<std::streamsize>(chunk));
            if (!file) {
                return fail(ErrorCode::IoError, "secure delete write failed");
            }
            remaining -= chunk;
        }
        file.flush();
    }

    file.close();

    if (options.removeAfterOverwrite) {
        std::filesystem::remove(path, ec);
        if (ec) { return fail(ErrorCode::IoError, ec.message()); }
    }

    return size;
}

}
