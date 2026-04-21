function(vk_add_static_library name)
    set(options "")
    set(oneValueArgs "")
    set(multiValueArgs SOURCES HEADERS PUBLIC_INCLUDES PRIVATE_INCLUDES PUBLIC_LIBS PRIVATE_LIBS)
    cmake_parse_arguments(VKLIB "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    add_library(${name} STATIC ${VKLIB_SOURCES} ${VKLIB_HEADERS})
    add_library(vk::${name} ALIAS ${name})

    target_include_directories(${name}
        PUBLIC ${VKLIB_PUBLIC_INCLUDES}
        PRIVATE ${VKLIB_PRIVATE_INCLUDES})

    target_link_libraries(${name}
        PUBLIC ${VKLIB_PUBLIC_LIBS}
        PRIVATE ${VKLIB_PRIVATE_LIBS})

    vk_apply_compiler_flags(${name})

    set_target_properties(${name} PROPERTIES
        FOLDER "Vaulkeeper"
        POSITION_INDEPENDENT_CODE ON)
endfunction()

function(vk_add_executable name)
    set(options CONSOLE GUI)
    set(oneValueArgs "")
    set(multiValueArgs SOURCES HEADERS RESOURCES PUBLIC_LIBS PRIVATE_LIBS PRIVATE_INCLUDES)
    cmake_parse_arguments(VKEXE "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    add_executable(${name} ${VKEXE_SOURCES} ${VKEXE_HEADERS} ${VKEXE_RESOURCES})
    add_executable(vk::${name} ALIAS ${name})

    target_include_directories(${name} PRIVATE ${VKEXE_PRIVATE_INCLUDES})

    target_link_libraries(${name}
        PUBLIC ${VKEXE_PUBLIC_LIBS}
        PRIVATE ${VKEXE_PRIVATE_LIBS})

    vk_apply_compiler_flags(${name})

    if(VKEXE_GUI)
        set_target_properties(${name} PROPERTIES
            WIN32_EXECUTABLE TRUE
            FOLDER "Vaulkeeper")
    else()
        set_target_properties(${name} PROPERTIES
            WIN32_EXECUTABLE FALSE
            FOLDER "Vaulkeeper")
    endif()
endfunction()

function(vk_add_test name)
    set(options "")
    set(oneValueArgs "")
    set(multiValueArgs SOURCES LIBS)
    cmake_parse_arguments(VKTEST "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    add_executable(${name} ${VKTEST_SOURCES})
    target_link_libraries(${name} PRIVATE ${VKTEST_LIBS})
    vk_apply_compiler_flags(${name})
    set_target_properties(${name} PROPERTIES FOLDER "Tests")
    add_test(NAME ${name} COMMAND ${name})
endfunction()
