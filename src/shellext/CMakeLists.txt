if(NOT WIN32)
    return()
endif()

set(VK_SHELLEXT_SOURCES
    dllmain.cpp
    context_menu.hpp
    context_menu.cpp
    class_factory.hpp
    class_factory.cpp
    shellext.def)

add_library(vaulkeeper_shellext SHARED ${VK_SHELLEXT_SOURCES})
add_library(vk::shellext ALIAS vaulkeeper_shellext)

target_include_directories(vaulkeeper_shellext PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(vaulkeeper_shellext PRIVATE shlwapi advapi32 ole32)
vk_apply_compiler_flags(vaulkeeper_shellext)

set_target_properties(vaulkeeper_shellext PROPERTIES
    FOLDER "Vaulkeeper"
    OUTPUT_NAME "vaulkeeper.shellext"
    PREFIX ""
    LINK_FLAGS "/DEF:\"${CMAKE_CURRENT_SOURCE_DIR}/shellext.def\"")
