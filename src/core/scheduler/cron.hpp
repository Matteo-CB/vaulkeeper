#pragma once

#include <array>
#include <bitset>
#include <chrono>
#include <optional>
#include <string_view>

#include "util/result.hpp"

namespace vk::core {

struct CronExpression {
    std::bitset<60> minutes;
    std::bitset<24> hours;
    std::bitset<31> daysOfMonth;
    std::bitset<12> months;
    std::bitset<7> daysOfWeek;
};

[[nodiscard]] Result<CronExpression> parseCron(std::string_view expression);
[[nodiscard]] std::optional<std::chrono::system_clock::time_point> nextFire(const CronExpression& expr, std::chrono::system_clock::time_point from);

}
