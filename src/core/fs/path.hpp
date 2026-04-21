#pragma once

#include <filesystem>
#include <string>
#include <string_view>

namespace vk::core {

[[nodiscard]] std::filesystem::path canonicalizeOrSelf(const std::filesystem::path& input);
[[nodiscard]] bool isWithinRoot(const std::filesystem::path& path, const std::filesystem::path& root);
[[nodiscard]] std::string toUtf8(const std::filesystem::path& p);
[[nodiscard]] std::filesystem::path fromUtf8(std::string_view s);
[[nodiscard]] std::wstring toExtendedPath(const std::filesystem::path& p);
[[nodiscard]] bool isReservedWindowsName(std::string_view name) noexcept;
[[nodiscard]] bool hasUnicodeSurrogate(std::string_view name) noexcept;

}
