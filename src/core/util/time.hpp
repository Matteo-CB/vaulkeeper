#pragma once

#include <chrono>
#include <cstdint>
#include <string>

namespace vk::core {

using SystemClock = std::chrono::system_clock;
using SteadyClock = std::chrono::steady_clock;
using TimePoint = SystemClock::time_point;
using SteadyTimePoint = SteadyClock::time_point;
using Duration = std::chrono::nanoseconds;

[[nodiscard]] std::int64_t toUnixNanos(TimePoint tp) noexcept;
[[nodiscard]] TimePoint fromUnixNanos(std::int64_t nanos) noexcept;
[[nodiscard]] std::string formatIso8601(TimePoint tp);
[[nodiscard]] std::string formatIso8601Utc(TimePoint tp);
[[nodiscard]] std::string formatDuration(Duration d);

class ScopedTimer {
public:
    explicit ScopedTimer(std::string label);
    ~ScopedTimer();

    ScopedTimer(const ScopedTimer&) = delete;
    ScopedTimer& operator=(const ScopedTimer&) = delete;

    [[nodiscard]] Duration elapsed() const noexcept;

private:
    std::string name;
    SteadyTimePoint start;
};

}
