#include <atomic>

#include <catch2/catch_all.hpp>

#include "util/thread_pool.hpp"

using namespace vk::core;

TEST_CASE("thread pool runs submitted work", "[util][thread_pool]") {
    ThreadPool pool { 4 };
    std::atomic<int> counter { 0 };
    for (int i = 0; i < 256; ++i) {
        pool.submit([&] { counter.fetch_add(1); });
    }
    pool.waitIdle();
    REQUIRE(counter.load() == 256);
}
