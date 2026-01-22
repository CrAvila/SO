cmake_minimum_required(VERSION 3.20)

project(FileForge
    VERSION 1.0.0
    DESCRIPTION "Forging Order from Chaos - A high-performance file organization utility"
    LANGUAGES CXX
)

# C++20 Standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Export compile commands for IDE support
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Options
option(FILEFORGE_BUILD_CLI "Build the CLI application" ON)
option(FILEFORGE_BUILD_GUI "Build the GUI application" ON)
option(FILEFORGE_BUILD_TESTS "Build unit tests" ON)
option(FILEFORGE_USE_SYSTEM_JSON "Use system nlohmann_json" OFF)
option(FILEFORGE_USE_SYSTEM_CLI11 "Use system CLI11" OFF)

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Compiler warnings
if(MSVC)
    add_compile_options(/W4 /WX /permissive-)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()

# Platform-specific settings
if(WIN32)
    add_definitions(-DFILEFORGE_PLATFORM_WINDOWS)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    add_definitions(-DNOMINMAX)
elseif(APPLE)
    add_definitions(-DFILEFORGE_PLATFORM_MACOS)
elseif(UNIX)
    add_definitions(-DFILEFORGE_PLATFORM_LINUX)
endif()

# Fetch dependencies
include(FetchContent)

# nlohmann/json
if(NOT FILEFORGE_USE_SYSTEM_JSON)
    FetchContent_Declare(
        nlohmann_json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG v3.11.3
    )
    FetchContent_MakeAvailable(nlohmann_json)
else()
    find_package(nlohmann_json REQUIRED)
endif()

# CLI11
if(NOT FILEFORGE_USE_SYSTEM_CLI11)
    FetchContent_Declare(
        CLI11
        GIT_REPOSITORY https://github.com/CLIUtils/CLI11.git
        GIT_TAG v2.4.2
    )
    FetchContent_MakeAvailable(CLI11)
else()
    find_package(CLI11 REQUIRED)
endif()

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# Core library
add_subdirectory(src/core)

# CLI application
if(FILEFORGE_BUILD_CLI)
    add_subdirectory(src/cli)
endif()

# GUI application (requires Qt6)
if(FILEFORGE_BUILD_GUI)
    find_package(Qt6 COMPONENTS Widgets QUIET)
    if(Qt6_FOUND)
        add_subdirectory(src/gui)
    else()
        message(STATUS "Qt6 not found - GUI will not be built")
    endif()
endif()

# Tests
if(FILEFORGE_BUILD_TESTS)
    enable_testing()
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.14.0
    )
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
    add_subdirectory(tests)
endif()

# Installation
install(TARGETS fileforge-core
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(DIRECTORY include/fileforge
    DESTINATION include
)

if(FILEFORGE_BUILD_CLI)
    install(TARGETS fileforge-cli
        RUNTIME DESTINATION bin
    )
endif()

if(FILEFORGE_BUILD_GUI AND Qt6_FOUND)
    install(TARGETS fileforge-gui
        RUNTIME DESTINATION bin
    )
endif()

# Package configuration
include(CMakePackageConfigHelpers)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/FileForgeConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

# Print configuration summary
message(STATUS "")
message(STATUS "FileForge ${PROJECT_VERSION} Configuration:")
message(STATUS "  Build CLI: ${FILEFORGE_BUILD_CLI}")
message(STATUS "  Build GUI: ${FILEFORGE_BUILD_GUI}")
message(STATUS "  Build Tests: ${FILEFORGE_BUILD_TESTS}")
message(STATUS "  C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "")
