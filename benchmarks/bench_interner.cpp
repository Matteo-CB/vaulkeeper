#include <string>

#include <benchmark/benchmark.h>

#include "util/string_intern.hpp"

static void BenchIntern(benchmark::State& state) {
    vk::core::StringInterner interner;
    for (auto _ : state) {
        const auto handle = interner.intern("C:/Users/tester/Documents/report.docx");
        benchmark::DoNotOptimize(handle);
    }
}
BENCHMARK(BenchIntern);
