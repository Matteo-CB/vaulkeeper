#pragma once

#include <atomic>
#include <condition_variable>
#include <cstddef>
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

#include "util/macros.hpp"

namespace vk::core {

enum class JobPriority : std::uint8_t {
    Interactive = 0,
    UserBackground = 1,
    LowPriority = 2,
    Idle = 3
};

struct JobTicket {
    std::size_t id { 0 };
    JobPriority priority { JobPriority::UserBackground };
};

class ThreadPool {
public:
    explicit ThreadPool(std::size_t workerCount = 0);
    ~ThreadPool();

    VK_NONCOPYABLE(ThreadPool);
    VK_NONMOVABLE(ThreadPool);

    void submit(std::function<void()> task, JobPriority priority = JobPriority::UserBackground);

    template <typename F>
    [[nodiscard]] auto submitWithResult(F&& fn, JobPriority priority = JobPriority::UserBackground)
        -> std::future<std::invoke_result_t<F>> {
        using Return = std::invoke_result_t<F>;
        auto task = std::make_shared<std::packaged_task<Return()>>(std::forward<F>(fn));
        auto future = task->get_future();
        submit([task] { (*task)(); }, priority);
        return future;
    }

    [[nodiscard]] std::size_t workerCount() const noexcept;
    [[nodiscard]] std::size_t pendingJobs() const noexcept;

    void waitIdle();

private:
    struct Job {
        JobPriority priority;
        std::uint64_t sequence;
        std::function<void()> fn;

        [[nodiscard]] bool operator<(const Job& other) const noexcept {
            if (priority != other.priority) {
                return static_cast<std::uint8_t>(priority) > static_cast<std::uint8_t>(other.priority);
            }
            return sequence > other.sequence;
        }
    };

    std::vector<std::thread> workers;
    std::priority_queue<Job> queue;
    std::mutex mutex;
    std::condition_variable notEmpty;
    std::condition_variable idle;
    std::atomic<bool> stopFlag { false };
    std::atomic<std::size_t> activeWorkers { 0 };
    std::uint64_t nextSequence { 0 };

    void workerLoop();
};

}
