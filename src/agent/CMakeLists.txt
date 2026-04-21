set(VK_AGENT_SOURCES
    main.cpp
    elevated/service.hpp
    elevated/service.cpp
    elevated/registry_ops.hpp
    elevated/registry_ops.cpp)

vk_add_executable(VaulkeeperAgent
    CONSOLE
    SOURCES ${VK_AGENT_SOURCES}
    PRIVATE_LIBS vk::vk_core vk::vk_platform_win
    PRIVATE_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR})

set_target_properties(VaulkeeperAgent PROPERTIES OUTPUT_NAME "VaulkeeperAgent")
