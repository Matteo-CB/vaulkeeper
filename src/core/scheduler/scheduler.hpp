#pragma once

#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include "scheduler/cron.hpp"
#include "util/macros.hpp"

namespace vk::core {

struct ScheduledTask {
    std::string id;
    CronExpression cron;
    std::function<void()> action;
    std::chrono::system_clock::time_point nextRun;
};

class Scheduler {
public:
    Scheduler();
    ~Scheduler();

    VK_NONCOPYABLE(Scheduler);
    VK_NONMOVABLE(Scheduler);

    void schedule(std::string id, CronExpression expr, std::function<void()> action);
    void cancel(std::string_view id);
    void start();
    void stop();

private:
    std::vector<ScheduledTask> tasks;
    std::mutex mutex;
    std::condition_variable signal;
    std::thread worker;
    std::atomic<bool> running { false };

    void loop();
};

}
