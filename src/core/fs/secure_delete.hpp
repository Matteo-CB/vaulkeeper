#pragma once

#include <cstdint>
#include <filesystem>

#include "util/result.hpp"

namespace vk::core {

enum class SecureDeleteStandard : std::uint8_t {
    Zero,
    Dod522022M,
    Gutmann
};

struct SecureDeleteOptions {
    SecureDeleteStandard standard { SecureDeleteStandard::Dod522022M };
    bool removeAfterOverwrite { true };
    std::uint32_t bufferSize { 1 << 20 };
};

[[nodiscard]] Result<std::uint64_t> secureDeleteFile(const std::filesystem::path& path, const SecureDeleteOptions& options);

}
