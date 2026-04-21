#pragma once

#include <cstdint>
#include <functional>
#include <string>
#include <vector>

#include "util/result.hpp"

namespace vk::platform::windows {

struct UsnChange {
    std::uint64_t recordNumber;
    std::uint64_t parentRecordNumber;
    std::uint64_t usn;
    std::uint32_t reason;
    std::uint32_t fileAttributes;
    std::wstring fileName;
};

[[nodiscard]] core::Result<std::vector<UsnChange>> readUsnJournal(const std::wstring& volume, std::uint64_t sinceUsn);

}
