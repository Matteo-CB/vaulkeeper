#include "scheduler/scheduler.hpp"

#include <algorithm>

#include <spdlog/spdlog.h>

namespace vk::core {

Scheduler::Scheduler() = default;

Scheduler::~Scheduler() {
    stop();
}

void Scheduler::schedule(std::string id, CronExpression expr, std::function<void()> action) {
    std::scoped_lock lock(mutex);
    ScheduledTask task;
    task.id = std::move(id);
    task.cron = std::move(expr);
    task.action = std::move(action);
    auto next = nextFire(task.cron, std::chrono::system_clock::now());
    task.nextRun = next.value_or(std::chrono::system_clock::now() + std::chrono::hours { 24 });
    tasks.push_back(std::move(task));
    signal.notify_all();
}

void Scheduler::cancel(std::string_view id) {
    std::scoped_lock lock(mutex);
    tasks.erase(std::remove_if(tasks.begin(), tasks.end(),
                               [&](const ScheduledTask& t) { return t.id == id; }),
                tasks.end());
    signal.notify_all();
}

void Scheduler::start() {
    if (running.exchange(true)) { return; }
    worker = std::thread { [this] { loop(); } };
}

void Scheduler::stop() {
    if (!running.exchange(false)) { return; }
    signal.notify_all();
    if (worker.joinable()) { worker.join(); }
}

void Scheduler::loop() {
    while (running.load()) {
        std::unique_lock lock(mutex);
        if (tasks.empty()) {
            signal.wait_for(lock, std::chrono::minutes { 1 });
            continue;
        }

        auto it = std::min_element(tasks.begin(), tasks.end(),
                                   [](const ScheduledTask& a, const ScheduledTask& b) { return a.nextRun < b.nextRun; });
        const auto sleepUntil = it->nextRun;
        const auto id = it->id;
        const auto action = it->action;
        signal.wait_until(lock, sleepUntil, [this] { return !running.load(); });
        if (!running.load()) { return; }

        const auto now = std::chrono::system_clock::now();
        if (now >= sleepUntil) {
            lock.unlock();
            spdlog::info("scheduler firing task {}", id);
            if (action) { action(); }
            lock.lock();
            auto task = std::find_if(tasks.begin(), tasks.end(),
                                     [&](const ScheduledTask& t) { return t.id == id; });
            if (task != tasks.end()) {
                auto next = nextFire(task->cron, now + std::chrono::seconds { 1 });
                task->nextRun = next.value_or(now + std::chrono::hours { 24 });
            }
        }
    }
}

}
