set(VK_UPDATER_SOURCES
    main.cpp
    update_check.hpp
    update_check.cpp
    signature.hpp
    signature.cpp)

vk_add_executable(vk-updater
    CONSOLE
    SOURCES ${VK_UPDATER_SOURCES}
    PRIVATE_LIBS vk::vk_core vk::vk_platform_win fmt::fmt spdlog::spdlog
    PRIVATE_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR})

set_target_properties(vk-updater PROPERTIES OUTPUT_NAME "vk-updater")
