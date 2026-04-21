find_package(benchmark CONFIG REQUIRED)

add_executable(vk_bench_hash bench_hash.cpp)
target_link_libraries(vk_bench_hash PRIVATE vk::vk_core benchmark::benchmark_main)
vk_apply_compiler_flags(vk_bench_hash)

add_executable(vk_bench_interner bench_interner.cpp)
target_link_libraries(vk_bench_interner PRIVATE vk::vk_core benchmark::benchmark_main)
vk_apply_compiler_flags(vk_bench_interner)
