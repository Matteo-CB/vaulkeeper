#include <filesystem>
#include <fstream>

#include <catch2/catch_all.hpp>

#include "scan/scan_cache.hpp"
#include "scan/scan_engine.hpp"
#include "util/thread_pool.hpp"

using namespace vk::core;

TEST_CASE("scan engine walks a synthetic tree", "[integration][scan]") {
    const auto root = std::filesystem::temp_directory_path() / "vaulkeeper_scan_itest";
    std::filesystem::remove_all(root);
    std::filesystem::create_directories(root / "sub1");
    std::filesystem::create_directories(root / "sub2");
    for (int i = 0; i < 10; ++i) {
        std::ofstream { root / "sub1" / ("f" + std::to_string(i) + ".bin") } << std::string(128, 'a');
    }
    for (int i = 0; i < 5; ++i) {
        std::ofstream { root / "sub2" / ("g" + std::to_string(i) + ".bin") } << std::string(64, 'b');
    }

    auto pool = std::make_shared<ThreadPool>(2);
    auto cache = std::make_shared<ScanCache>();
    ScanEngine engine { pool, cache };

    ScanOptions opts;
    opts.roots = { root };
    auto result = engine.run(opts);
    REQUIRE(result.has_value());
    REQUIRE(result->stats.filesVisited >= 15);
    REQUIRE(result->stats.bytesIndexed >= 10 * 128 + 5 * 64);
}
