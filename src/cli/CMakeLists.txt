set(VK_CLI_SOURCES
    main.cpp
    commands/command.hpp
    commands/command.cpp
    commands/scan_cmd.hpp
    commands/scan_cmd.cpp
    commands/duplicates_cmd.hpp
    commands/duplicates_cmd.cpp
    commands/quarantine_cmd.hpp
    commands/quarantine_cmd.cpp
    commands/uninstall_cmd.hpp
    commands/uninstall_cmd.cpp
    commands/rules_cmd.hpp
    commands/rules_cmd.cpp
    commands/health_cmd.hpp
    commands/health_cmd.cpp
    commands/report_cmd.hpp
    commands/report_cmd.cpp)

vk_add_executable(vk
    CONSOLE
    SOURCES ${VK_CLI_SOURCES}
    PRIVATE_LIBS vk::vk_core vk::vk_platform_win fmt::fmt spdlog::spdlog
    PRIVATE_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR})

set_target_properties(vk PROPERTIES OUTPUT_NAME "vk")
