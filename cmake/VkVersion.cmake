set(VK_VERSION_MAJOR 0)
set(VK_VERSION_MINOR 1)
set(VK_VERSION_PATCH 0)
set(VK_VERSION_STRING "${VK_VERSION_MAJOR}.${VK_VERSION_MINOR}.${VK_VERSION_PATCH}")

find_package(Git QUIET)
if(GIT_FOUND)
    execute_process(
        COMMAND ${GIT_EXECUTABLE} rev-parse --short=12 HEAD
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE VK_GIT_SHA
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET)
    execute_process(
        COMMAND ${GIT_EXECUTABLE} rev-parse --abbrev-ref HEAD
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE VK_GIT_BRANCH
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET)
endif()

if(NOT VK_GIT_SHA)
    set(VK_GIT_SHA "unknown")
endif()
if(NOT VK_GIT_BRANCH)
    set(VK_GIT_BRANCH "unknown")
endif()

string(TIMESTAMP VK_BUILD_DATE "%Y-%m-%d")
string(TIMESTAMP VK_BUILD_TIME "%H:%M:%S" UTC)

configure_file(
    "${CMAKE_SOURCE_DIR}/cmake/version.hpp.in"
    "${CMAKE_BINARY_DIR}/generated/vk/util/version.hpp"
    @ONLY)

function(vk_print_summary)
    message(STATUS "")
    message(STATUS "Vaulkeeper ${VK_VERSION_STRING} (${VK_GIT_SHA} on ${VK_GIT_BRANCH})")
    message(STATUS "  Build type       : ${CMAKE_BUILD_TYPE}")
    message(STATUS "  Compiler         : ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
    message(STATUS "  Tests            : ${VK_BUILD_TESTS}")
    message(STATUS "  Benchmarks       : ${VK_BUILD_BENCHMARKS}")
    message(STATUS "  Installer        : ${VK_BUILD_INSTALLER}")
    message(STATUS "  LTO              : ${VK_ENABLE_LTO}")
    message(STATUS "  mimalloc         : ${VK_USE_MIMALLOC}")
    message(STATUS "  TBB              : ${VK_USE_TBB}")
    message(STATUS "  ASAN             : ${VK_ENABLE_ASAN}")
    message(STATUS "  UBSAN            : ${VK_ENABLE_UBSAN}")
    message(STATUS "")
endfunction()
