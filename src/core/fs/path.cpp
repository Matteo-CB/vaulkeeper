#include "fs/path.hpp"

#include <algorithm>
#include <array>
#include <cctype>

namespace vk::core {

namespace {

constexpr std::array<std::string_view, 22> ReservedNames {
    "CON", "PRN", "AUX", "NUL",
    "COM1", "COM2", "COM3", "COM4", "COM5", "COM6", "COM7", "COM8", "COM9",
    "LPT1", "LPT2", "LPT3", "LPT4", "LPT5", "LPT6", "LPT7", "LPT8", "LPT9"
};

bool iequals(std::string_view a, std::string_view b) {
    if (a.size() != b.size()) { return false; }
    for (std::size_t i = 0; i < a.size(); ++i) {
        const auto ca = static_cast<unsigned char>(a[i]);
        const auto cb = static_cast<unsigned char>(b[i]);
        if (std::toupper(ca) != std::toupper(cb)) { return false; }
    }
    return true;
}

}

std::filesystem::path canonicalizeOrSelf(const std::filesystem::path& input) {
    std::error_code ec;
    auto canonical = std::filesystem::weakly_canonical(input, ec);
    return ec ? input : canonical;
}

bool isWithinRoot(const std::filesystem::path& path, const std::filesystem::path& root) {
    auto canon = canonicalizeOrSelf(path);
    auto rootCanon = canonicalizeOrSelf(root);
    auto itPair = std::mismatch(canon.begin(), canon.end(), rootCanon.begin(), rootCanon.end());
    return itPair.second == rootCanon.end();
}

std::string toUtf8(const std::filesystem::path& p) {
    const auto u8 = p.u8string();
    return std::string(reinterpret_cast<const char*>(u8.data()), u8.size());
}

std::filesystem::path fromUtf8(std::string_view s) {
    std::u8string wide(reinterpret_cast<const char8_t*>(s.data()), s.size());
    return std::filesystem::path(wide);
}

std::wstring toExtendedPath(const std::filesystem::path& p) {
    auto wide = p.wstring();
    if (wide.size() >= 4 && wide.substr(0, 4) == L"\\\\?\\") {
        return wide;
    }
    if (wide.size() >= 2 && wide[0] == L'\\' && wide[1] == L'\\') {
        return L"\\\\?\\UNC" + wide.substr(1);
    }
    return L"\\\\?\\" + wide;
}

bool isReservedWindowsName(std::string_view name) noexcept {
    const auto dot = name.find('.');
    const auto stem = dot == std::string_view::npos ? name : name.substr(0, dot);
    for (auto r : ReservedNames) {
        if (iequals(stem, r)) { return true; }
    }
    return false;
}

bool hasUnicodeSurrogate(std::string_view name) noexcept {
    for (std::size_t i = 0; i + 2 < name.size(); ++i) {
        const auto b0 = static_cast<unsigned char>(name[i]);
        const auto b1 = static_cast<unsigned char>(name[i + 1]);
        const auto b2 = static_cast<unsigned char>(name[i + 2]);
        if (b0 == 0xED && (b1 & 0xE0) == 0xA0 && b2 >= 0x80 && b2 <= 0xBF) {
            return true;
        }
    }
    return false;
}

}
