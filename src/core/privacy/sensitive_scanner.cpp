#include "privacy/sensitive_scanner.hpp"

#include <array>
#include <fstream>
#include <regex>
#include <string>

namespace vk::core {

namespace {

struct Pattern {
    SensitivePatternKind kind;
    std::regex regex;
};

const std::array<Pattern, 7>& patterns() {
    static const std::array<Pattern, 7> all {{
        { SensitivePatternKind::Email,      std::regex { R"([\w.+-]+@[A-Za-z0-9.-]+\.[A-Za-z]{2,})" } },
        { SensitivePatternKind::Iban,       std::regex { R"([A-Z]{2}\d{2}[A-Z0-9]{11,30})" } },
        { SensitivePatternKind::CreditCard, std::regex { R"(\b(?:\d[ -]?){13,19}\b)" } },
        { SensitivePatternKind::PhoneNumber,std::regex { R"(\+?\d{1,3}[ -.]?\(?\d{1,4}\)?[ -.]?\d{3,4}[ -.]?\d{3,4})" } },
        { SensitivePatternKind::Ssn,        std::regex { R"(\b\d{3}[- ]?\d{2}[- ]?\d{4}\b)" } },
        { SensitivePatternKind::PrivateKey, std::regex { R"(-----BEGIN (RSA|EC|DSA|OPENSSH|PGP) PRIVATE KEY-----)" } },
        { SensitivePatternKind::ApiToken,   std::regex { R"((?:sk_live|pk_live|AKIA|ghp_|gho_|ghs_|xox[baprs]-)[A-Za-z0-9_\-]{16,})" } }
    }};
    return all;
}

bool kindEnabled(const SensitiveScanOptions& options, SensitivePatternKind kind) {
    if (options.patterns.empty()) { return true; }
    for (auto pattern : options.patterns) {
        if (pattern == kind) { return true; }
    }
    return false;
}

bool looksLikeBinary(std::ifstream& stream) {
    char probe[512];
    stream.read(probe, sizeof(probe));
    const auto read = stream.gcount();
    stream.clear();
    stream.seekg(0);
    for (std::streamsize i = 0; i < read; ++i) {
        if (probe[i] == '\0') { return true; }
    }
    return false;
}

}

Result<std::vector<SensitiveMatch>> scanForSensitiveData(const std::vector<std::filesystem::path>& targets, const SensitiveScanOptions& options) {
    std::vector<SensitiveMatch> matches;
    std::size_t scanned = 0;
    const auto total = targets.size();

    for (const auto& file : targets) {
        ++scanned;
        if (options.progress) { options.progress(scanned, total); }

        std::error_code ec;
        const auto size = std::filesystem::file_size(file, ec);
        if (ec || size > options.maxFileSize) { continue; }

        std::ifstream stream(file, std::ios::binary);
        if (!stream) { continue; }

        if (!options.includeBinaries && looksLikeBinary(stream)) { continue; }

        std::string line;
        std::uint64_t lineNo = 0;
        while (std::getline(stream, line)) {
            ++lineNo;
            for (const auto& pattern : patterns()) {
                if (!kindEnabled(options, pattern.kind)) { continue; }
                std::smatch match;
                if (std::regex_search(line, match, pattern.regex)) {
                    SensitiveMatch hit;
                    hit.file = file;
                    hit.lineNumber = lineNo;
                    hit.kind = pattern.kind;
                    hit.preview = match.str();
                    matches.push_back(std::move(hit));
                }
            }
        }
    }

    return matches;
}

}
