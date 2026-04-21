cmake_minimum_required(VERSION 3.28)

set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
    CACHE STRING "vcpkg toolchain file")

project(vaulkeeper
    VERSION 0.1.0
    DESCRIPTION "Native Windows storage manager"
    LANGUAGES CXX C)

if(NOT WIN32)
    message(FATAL_ERROR "Vaulkeeper targets Windows only. Current platform is not supported.")
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

include(VkOptions)
include(VkCompilerFlags)
include(VkTargets)
include(VkVersion)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS Debug Release RelWithDebInfo MinSizeRel)
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

find_package(Qt6 6.7 REQUIRED COMPONENTS Core Gui Quick QuickControls2 Widgets Svg LinguistTools Concurrent Network)
find_package(unofficial-sqlite3 CONFIG REQUIRED)
find_package(xxHash CONFIG REQUIRED)
find_package(unofficial-blake3 CONFIG REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(LibArchive REQUIRED)
find_package(zstd CONFIG REQUIRED)
find_package(mimalloc CONFIG REQUIRED)
find_package(TBB CONFIG REQUIRED)
find_package(absl CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(tomlplusplus CONFIG REQUIRED)
find_package(yaml-cpp CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(Lua CONFIG REQUIRED)

qt_standard_project_setup(REQUIRES 6.7)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

enable_testing()

add_subdirectory(src/core)
add_subdirectory(src/platform/windows)
add_subdirectory(src/app)
add_subdirectory(src/cli)
add_subdirectory(src/agent)
add_subdirectory(src/shellext)
add_subdirectory(src/updater)

if(VK_BUILD_TESTS)
    add_subdirectory(tests)
endif()

if(VK_BUILD_BENCHMARKS)
    add_subdirectory(benchmarks)
endif()

if(VK_BUILD_INSTALLER)
    add_subdirectory(src/installer)
endif()

vk_print_summary()
