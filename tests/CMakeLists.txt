find_package(Catch2 3 CONFIG REQUIRED)

set(VK_UNIT_SOURCES
    unit/test_main.cpp
    unit/util/test_bytes.cpp
    unit/util/test_time.cpp
    unit/util/test_string_intern.cpp
    unit/util/test_mpsc_queue.cpp
    unit/util/test_thread_pool.cpp
    unit/util/test_secure_string.cpp
    unit/hash/test_xxhash3.cpp
    unit/hash/test_blake3.cpp
    unit/hash/test_phash.cpp
    unit/scan/test_tree_model.cpp
    unit/duplicates/test_duplicate_finder.cpp
    unit/rules/test_rule_parser.cpp
    unit/scheduler/test_cron.cpp
    unit/ipc/test_message.cpp
    unit/quarantine/test_quarantine.cpp
    unit/privacy/test_sensitive_scanner.cpp
    unit/fs/test_path.cpp
    unit/fs/test_secure_delete.cpp
    unit/fs/test_compression.cpp
    unit/uninstall/test_residue_scanner.cpp
    unit/cleaners/test_dev_artifact_cleaner.cpp)

vk_add_test(vk_unit_tests
    SOURCES ${VK_UNIT_SOURCES}
    LIBS vk::vk_core Catch2::Catch2WithMain fmt::fmt spdlog::spdlog)

set(VK_INTEGRATION_SOURCES
    integration/test_main.cpp
    integration/test_scan_pipeline.cpp
    integration/test_quarantine_roundtrip.cpp
    integration/test_rule_execution.cpp)

vk_add_test(vk_integration_tests
    SOURCES ${VK_INTEGRATION_SOURCES}
    LIBS vk::vk_core vk::vk_platform_win Catch2::Catch2WithMain)

set(VK_FUZZ_SOURCES
    fuzz/fuzz_mft.cpp
    fuzz/fuzz_cron.cpp
    fuzz/fuzz_rules.cpp
    fuzz/fuzz_ipc.cpp)

vk_add_test(vk_fuzz_smoke
    SOURCES ${VK_FUZZ_SOURCES}
    LIBS vk::vk_core vk::vk_platform_win Catch2::Catch2WithMain)

set(VK_PERF_SOURCES
    perf/perf_scan.cpp
    perf/perf_hash.cpp)

vk_add_test(vk_perf_smoke
    SOURCES ${VK_PERF_SOURCES}
    LIBS vk::vk_core Catch2::Catch2WithMain)

add_executable(vk_updater_tests
    unit/test_main.cpp
    unit/updater/test_update_check.cpp
    ${CMAKE_SOURCE_DIR}/src/updater/update_check.cpp)
target_include_directories(vk_updater_tests PRIVATE ${CMAKE_SOURCE_DIR}/src/updater)
target_link_libraries(vk_updater_tests PRIVATE vk::vk_core Catch2::Catch2WithMain fmt::fmt winhttp)
vk_apply_compiler_flags(vk_updater_tests)
set_target_properties(vk_updater_tests PROPERTIES FOLDER "Tests")
add_test(NAME vk_updater_tests COMMAND vk_updater_tests)
