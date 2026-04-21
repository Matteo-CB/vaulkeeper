#pragma once

#include <cstdint>
#include <filesystem>
#include <functional>
#include <string>
#include <vector>

#include "util/result.hpp"

namespace vk::core {

enum class SensitivePatternKind : std::uint8_t {
    Iban,
    CreditCard,
    Ssn,
    Email,
    PhoneNumber,
    PrivateKey,
    ApiToken
};

struct SensitiveMatch {
    std::filesystem::path file;
    std::uint64_t lineNumber { 0 };
    SensitivePatternKind kind;
    std::string preview;
};

struct SensitiveScanOptions {
    std::vector<SensitivePatternKind> patterns;
    bool includeBinaries { false };
    std::uint64_t maxFileSize { 32ULL * 1024ULL * 1024ULL };
    std::function<void(std::size_t scanned, std::size_t total)> progress;
};

[[nodiscard]] Result<std::vector<SensitiveMatch>> scanForSensitiveData(const std::vector<std::filesystem::path>& targets, const SensitiveScanOptions& options);

}
