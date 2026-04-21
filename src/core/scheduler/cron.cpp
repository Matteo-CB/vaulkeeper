#include "scheduler/cron.hpp"

#include <algorithm>
#include <charconv>
#include <ctime>
#include <sstream>
#include <vector>

namespace vk::core {

namespace {

Result<std::vector<int>> parseField(std::string_view field, int minValue, int maxValue) {
    std::vector<int> values;
    if (field == "*") {
        for (int v = minValue; v <= maxValue; ++v) values.push_back(v);
        return values;
    }

    std::string s(field);
    std::replace(s.begin(), s.end(), ',', ' ');
    std::stringstream ss(s);
    std::string part;
    while (ss >> part) {
        int step = 1;
        std::string rangePart = part;
        if (const auto slash = part.find('/'); slash != std::string::npos) {
            rangePart = part.substr(0, slash);
            auto stepStr = part.substr(slash + 1);
            auto [p, ec] = std::from_chars(stepStr.data(), stepStr.data() + stepStr.size(), step);
            if (ec != std::errc {}) { return fail(ErrorCode::InvalidArgument, "bad cron step"); }
        }

        int lo = minValue;
        int hi = maxValue;
        if (rangePart == "*") {
            // already lo/hi
        } else if (const auto dash = rangePart.find('-'); dash != std::string::npos) {
            const auto loStr = rangePart.substr(0, dash);
            const auto hiStr = rangePart.substr(dash + 1);
            std::from_chars(loStr.data(), loStr.data() + loStr.size(), lo);
            std::from_chars(hiStr.data(), hiStr.data() + hiStr.size(), hi);
        } else {
            std::from_chars(rangePart.data(), rangePart.data() + rangePart.size(), lo);
            hi = lo;
        }
        if (lo < minValue || hi > maxValue || lo > hi || step < 1) {
            return fail(ErrorCode::InvalidArgument, "cron value out of bounds");
        }
        for (int v = lo; v <= hi; v += step) { values.push_back(v); }
    }
    return values;
}

template <std::size_t N>
Result<void> fillField(std::bitset<N>& target, std::string_view field, int minValue, int maxValue) {
    auto values = parseField(field, minValue, maxValue);
    if (!values) { return fail(values.error()); }
    for (int v : *values) {
        if (v >= 0 && static_cast<std::size_t>(v) < N) {
            target.set(v);
        }
    }
    return ok();
}

}

Result<CronExpression> parseCron(std::string_view expression) {
    std::vector<std::string> parts;
    std::stringstream ss { std::string(expression) };
    std::string token;
    while (ss >> token) { parts.push_back(token); }
    if (parts.size() != 5) { return fail(ErrorCode::InvalidArgument, "cron expects 5 fields"); }

    CronExpression expr;
    if (auto r = fillField(expr.minutes, parts[0], 0, 59); !r) { return fail(r.error()); }
    if (auto r = fillField(expr.hours, parts[1], 0, 23); !r) { return fail(r.error()); }
    if (auto r = fillField(expr.daysOfMonth, parts[2], 1, 31); !r) { return fail(r.error()); }
    if (auto r = fillField(expr.months, parts[3], 1, 12); !r) { return fail(r.error()); }
    if (auto r = fillField(expr.daysOfWeek, parts[4], 0, 6); !r) { return fail(r.error()); }
    return expr;
}

std::optional<std::chrono::system_clock::time_point> nextFire(const CronExpression& expr, std::chrono::system_clock::time_point from) {
    auto t = std::chrono::system_clock::to_time_t(from);
    std::tm local {};
#ifdef _WIN32
    localtime_s(&local, &t);
#else
    localtime_r(&t, &local);
#endif
    local.tm_sec = 0;
    ++local.tm_min;

    for (int attempt = 0; attempt < 366 * 24 * 60; ++attempt) {
        const auto month = local.tm_mon;
        const auto dayOfMonth = local.tm_mday - 1;
        const auto dow = local.tm_wday;
        if (expr.months.test(static_cast<std::size_t>(month)) &&
            expr.daysOfMonth.test(static_cast<std::size_t>(dayOfMonth)) &&
            expr.daysOfWeek.test(static_cast<std::size_t>(dow)) &&
            expr.hours.test(static_cast<std::size_t>(local.tm_hour)) &&
            expr.minutes.test(static_cast<std::size_t>(local.tm_min))) {
            const auto fire = std::mktime(&local);
            return std::chrono::system_clock::from_time_t(fire);
        }
        ++local.tm_min;
        std::mktime(&local);
    }
    return std::nullopt;
}

}
