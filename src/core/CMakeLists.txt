set(VK_CORE_SOURCES
    util/macros.hpp
    util/error.hpp
    util/error.cpp
    util/result.hpp
    util/string_intern.hpp
    util/string_intern.cpp
    util/bytes.hpp
    util/time.hpp
    util/time.cpp
    util/logger.hpp
    util/logger.cpp
    util/thread_pool.hpp
    util/thread_pool.cpp
    util/mpsc_queue.hpp
    util/event_bus.hpp
    util/event_bus.cpp
    util/secure_string.hpp
    util/secure_string.cpp

    config/app_config.hpp
    config/app_config.cpp
    config/paths.hpp
    config/paths.cpp

    fs/path.hpp
    fs/path.cpp
    fs/file_entry.hpp
    fs/volume.hpp
    fs/volume.cpp
    fs/compression.hpp
    fs/compression.cpp
    fs/secure_delete.hpp
    fs/secure_delete.cpp
    fs/reparse.hpp
    fs/reparse.cpp

    hash/xxhash3.hpp
    hash/xxhash3.cpp
    hash/blake3_hash.hpp
    hash/blake3_hash.cpp
    hash/phash.hpp
    hash/phash.cpp

    scan/scan_options.hpp
    scan/scan_result.hpp
    scan/scan_engine.hpp
    scan/scan_engine.cpp
    scan/scan_cache.hpp
    scan/scan_cache.cpp
    scan/tree_model.hpp
    scan/tree_model.cpp

    duplicates/duplicate_finder.hpp
    duplicates/duplicate_finder.cpp

    rules/rule.hpp
    rules/rule_engine.hpp
    rules/rule_engine.cpp
    rules/rule_parser.hpp
    rules/rule_parser.cpp

    quarantine/quarantine.hpp
    quarantine/quarantine.cpp

    journal/journal.hpp
    journal/journal.cpp

    uninstall/application.hpp
    uninstall/uninstaller.hpp
    uninstall/uninstaller.cpp
    uninstall/residue_scanner.hpp
    uninstall/residue_scanner.cpp

    cleaners/cleaner.hpp
    cleaners/cleaner_registry.hpp
    cleaners/cleaner_registry.cpp
    cleaners/temp_cleaner.hpp
    cleaners/temp_cleaner.cpp
    cleaners/browser_cache_cleaner.hpp
    cleaners/browser_cache_cleaner.cpp
    cleaners/dev_artifact_cleaner.hpp
    cleaners/dev_artifact_cleaner.cpp
    cleaners/recycle_bin_cleaner.hpp
    cleaners/recycle_bin_cleaner.cpp
    cleaners/windows_update_cleaner.hpp
    cleaners/windows_update_cleaner.cpp
    cleaners/thumbnail_cache_cleaner.hpp
    cleaners/thumbnail_cache_cleaner.cpp
    cleaners/crash_dump_cleaner.hpp
    cleaners/crash_dump_cleaner.cpp

    health/disk_health.hpp
    health/disk_health.cpp

    privacy/sensitive_scanner.hpp
    privacy/sensitive_scanner.cpp

    scheduler/scheduler.hpp
    scheduler/scheduler.cpp
    scheduler/cron.hpp
    scheduler/cron.cpp

    ipc/pipe_server.hpp
    ipc/pipe_server.cpp
    ipc/pipe_client.hpp
    ipc/pipe_client.cpp
    ipc/message.hpp
    ipc/message.cpp

    plugin/plugin_host.hpp
    plugin/plugin_host.cpp
    plugin/plugin_api.hpp

    lua/lua_runtime.hpp
    lua/lua_runtime.cpp)

vk_add_static_library(vk_core
    SOURCES ${VK_CORE_SOURCES}
    PUBLIC_INCLUDES
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_BINARY_DIR}/generated
    PUBLIC_LIBS
        fmt::fmt
        spdlog::spdlog
        absl::flat_hash_map
        absl::flat_hash_set
        absl::strings
    PRIVATE_LIBS
        xxHash::xxhash
        unofficial::blake3::blake3
        unofficial::sqlite3::sqlite3
        yaml-cpp::yaml-cpp
        tomlplusplus::tomlplusplus
        zstd::libzstd
        TBB::tbb
        ${LUA_LIBRARIES})
