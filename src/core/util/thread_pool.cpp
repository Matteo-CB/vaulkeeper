#include "util/thread_pool.hpp"

#include <algorithm>

namespace vk::core {

ThreadPool::ThreadPool(std::size_t workerCount) {
    const auto hw = std::max<std::size_t>(1, std::thread::hardware_concurrency());
    const auto count = workerCount == 0 ? std::max<std::size_t>(1, hw - 1) : workerCount;
    workers.reserve(count);
    for (std::size_t i = 0; i < count; ++i) {
        workers.emplace_back([this] { workerLoop(); });
    }
}

ThreadPool::~ThreadPool() {
    {
        std::scoped_lock lock(mutex);
        stopFlag.store(true);
    }
    notEmpty.notify_all();
    for (auto& t : workers) {
        if (t.joinable()) {
            t.join();
        }
    }
}

void ThreadPool::submit(std::function<void()> task, JobPriority priority) {
    {
        std::scoped_lock lock(mutex);
        queue.push(Job { priority, nextSequence++, std::move(task) });
    }
    notEmpty.notify_one();
}

std::size_t ThreadPool::workerCount() const noexcept {
    return workers.size();
}

std::size_t ThreadPool::pendingJobs() const noexcept {
    std::scoped_lock lock(const_cast<std::mutex&>(mutex));
    return queue.size();
}

void ThreadPool::waitIdle() {
    std::unique_lock lock(mutex);
    idle.wait(lock, [this] { return queue.empty() && activeWorkers.load() == 0; });
}

void ThreadPool::workerLoop() {
    while (true) {
        Job job;
        {
            std::unique_lock lock(mutex);
            notEmpty.wait(lock, [this] { return stopFlag.load() || !queue.empty(); });
            if (stopFlag.load() && queue.empty()) {
                return;
            }
            job = std::move(const_cast<Job&>(queue.top()));
            queue.pop();
            activeWorkers.fetch_add(1);
        }
        if (job.fn) {
            job.fn();
        }
        if (activeWorkers.fetch_sub(1) == 1) {
            std::scoped_lock lock(mutex);
            if (queue.empty()) {
                idle.notify_all();
            }
        }
    }
}

}
