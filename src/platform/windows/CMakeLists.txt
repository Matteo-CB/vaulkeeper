set(VK_WIN_SOURCES
    mft/ntfs_mft.hpp
    mft/ntfs_mft.cpp
    mft/usn_journal.hpp
    mft/usn_journal.cpp

    etw/etw_session.hpp
    etw/etw_session.cpp

    wmi/wmi_query.hpp
    wmi/wmi_query.cpp

    registry/registry.hpp
    registry/registry.cpp

    services/service_manager.hpp
    services/service_manager.cpp

    tasks/task_scheduler.hpp
    tasks/task_scheduler.cpp

    msix/msix_inventory.hpp
    msix/msix_inventory.cpp

    shell/shell_links.hpp
    shell/shell_links.cpp
    shell/known_folders.hpp
    shell/known_folders.cpp

    bitlocker/bitlocker.hpp
    bitlocker/bitlocker.cpp

    smart/smart_reader.hpp
    smart/smart_reader.cpp

    platform_init.hpp
    platform_init.cpp)

vk_add_static_library(vk_platform_win
    SOURCES ${VK_WIN_SOURCES}
    PUBLIC_INCLUDES
        ${CMAKE_CURRENT_SOURCE_DIR}
    PUBLIC_LIBS vk::vk_core
    PRIVATE_LIBS
        wbemuuid
        advapi32
        shell32
        ole32
        oleaut32
        version
        userenv
        ntdll
        taskschd
        tdh
        uuid)
