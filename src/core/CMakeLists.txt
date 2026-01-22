# FileForge Core Library

add_library(fileforge-core
    config.cpp
    logger.cpp
    scanner.cpp
    categorizer.cpp
    renamer.cpp
    organizer.cpp
)

target_include_directories(fileforge-core
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(fileforge-core
    PUBLIC
        nlohmann_json::nlohmann_json
)

# Platform-specific libraries
if(WIN32)
    target_link_libraries(fileforge-core PRIVATE shlwapi)
elseif(UNIX AND NOT APPLE)
    find_package(Threads REQUIRED)
    target_link_libraries(fileforge-core PRIVATE Threads::Threads)
endif()

# Set properties
set_target_properties(fileforge-core PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    POSITION_INDEPENDENT_CODE ON
)

# Alias for use in other targets
add_library(FileForge::Core ALIAS fileforge-core)
