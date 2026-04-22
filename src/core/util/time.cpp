#include "util/time.hpp"

#include <ctime>
#include <fmt/chrono.h>
#include <fmt/format.h>
#include <spdlog/spdlog.h>

namespace vk::core {

std::int64_t toUnixNanos(TimePoint tp) noexcept {
    return std::chrono::duration_cast<std::chrono::nanoseconds>(tp.time_since_epoch()).count();
}

TimePoint fromUnixNanos(std::int64_t nanos) noexcept {
    const auto asNs = std::chrono::nanoseconds { nanos };
    return TimePoint { std::chrono::duration_cast<SystemClock::duration>(asNs) };
}

std::string formatIso8601(TimePoint tp) {
    const auto asTime = SystemClock::to_time_t(tp);
    std::tm local {};
#ifdef _WIN32
    localtime_s(&local, &asTime);
#else
    localtime_r(&asTime, &local);
#endif
    char buffer[32];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%dT%H:%M:%S", &local);
    return std::string(buffer);
}

std::string formatIso8601Utc(TimePoint tp) {
    const auto asTime = SystemClock::to_time_t(tp);
    std::tm utc {};
#ifdef _WIN32
    gmtime_s(&utc, &asTime);
#else
    gmtime_r(&asTime, &utc);
#endif
    char buffer[32];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%dT%H:%M:%SZ", &utc);
    return std::string(buffer);
}

std::string formatDuration(Duration d) {
    using namespace std::chrono;
    const auto totalMicros = duration_cast<microseconds>(d).count();
    if (totalMicros < 1000) {
        return fmt::format("{} us", totalMicros);
    }
    if (totalMicros < 1'000'000) {
        return fmt::format("{:.2f} ms", static_cast<double>(totalMicros) / 1000.0);
    }
    const auto seconds = static_cast<double>(totalMicros) / 1'000'000.0;
    if (seconds < 60.0) {
        return fmt::format("{:.2f} s", seconds);
    }
    const auto minutes = seconds / 60.0;
    return fmt::format("{:.2f} min", minutes);
}

ScopedTimer::ScopedTimer(std::string label) : name(std::move(label)), start(SteadyClock::now()) {}

ScopedTimer::~ScopedTimer() {
    spdlog::debug("{} took {}", name, formatDuration(SteadyClock::now() - start));
}

Duration ScopedTimer::elapsed() const noexcept {
    return std::chrono::duration_cast<Duration>(SteadyClock::now() - start);
}

}
