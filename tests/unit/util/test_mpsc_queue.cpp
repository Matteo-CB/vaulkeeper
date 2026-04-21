#include <atomic>
#include <thread>

#include <catch2/catch_all.hpp>

#include "util/mpsc_queue.hpp"

using namespace vk::core;

TEST_CASE("mpsc queue preserves values", "[util][mpsc]") {
    MpscQueue<int> queue;
    std::atomic<int> sum { 0 };

    auto producer = [&] {
        for (int i = 0; i < 100; ++i) {
            queue.push(i);
        }
    };

    std::thread t1 { producer };
    std::thread t2 { producer };
    t1.join();
    t2.join();

    int collected = 0;
    int consumed = 0;
    while (auto value = queue.tryPop()) {
        collected += *value;
        ++consumed;
    }
    REQUIRE(consumed == 200);
    REQUIRE(collected == 2 * (99 * 100 / 2));
}
