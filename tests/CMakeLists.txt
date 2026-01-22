# FileForge Tests

include(GoogleTest)

# Test executable
add_executable(fileforge-tests
    test_types.cpp
    test_config.cpp
    test_categorizer.cpp
    test_renamer.cpp
    test_scanner.cpp
    test_organizer.cpp
)

target_link_libraries(fileforge-tests
    PRIVATE
        fileforge-core
        GTest::gtest
        GTest::gtest_main
)

target_include_directories(fileforge-tests
    PRIVATE
        ${CMAKE_SOURCE_DIR}/include
)

set_target_properties(fileforge-tests PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)

# Discover tests
gtest_discover_tests(fileforge-tests)

# Create test fixtures directory
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/test_fixtures)
