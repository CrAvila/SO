# FileForge CLI Application

add_executable(fileforge-cli
    main.cpp
)

target_link_libraries(fileforge-cli
    PRIVATE
        fileforge-core
        CLI11::CLI11
)

set_target_properties(fileforge-cli PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)

# Install target
install(TARGETS fileforge-cli
    RUNTIME DESTINATION bin
)
