#include <filesystem>
#include <fstream>

#include <catch2/benchmark/catch_benchmark_all.hpp>
#include <catch2/catch_all.hpp>

#include "scan/scan_cache.hpp"
#include "scan/scan_engine.hpp"
#include "util/thread_pool.hpp"

TEST_CASE("scan perf smoke", "[perf][scan][!benchmark]") {
    const auto root = std::filesystem::temp_directory_path() / "vaulkeeper_perf_scan";
    std::filesystem::remove_all(root);
    std::filesystem::create_directories(root);
    for (int i = 0; i < 500; ++i) {
        std::ofstream { root / ("f" + std::to_string(i) + ".bin") } << std::string(4096, 'x');
    }

    auto pool = std::make_shared<vk::core::ThreadPool>(4);
    auto cache = std::make_shared<vk::core::ScanCache>();
    vk::core::ScanEngine engine { pool, cache };

    BENCHMARK("scan 500 small files") {
        vk::core::ScanOptions opts;
        opts.roots = { root };
        return engine.run(opts);
    };
}
