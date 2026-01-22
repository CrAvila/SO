# FileForge GUI Application (Qt6)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

add_executable(fileforge-gui
    main.cpp
    mainwindow.cpp
    mainwindow.hpp
)

target_link_libraries(fileforge-gui
    PRIVATE
        fileforge-core
        Qt6::Widgets
)

set_target_properties(fileforge-gui PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    WIN32_EXECUTABLE ON
    MACOSX_BUNDLE ON
)

# Install target
install(TARGETS fileforge-gui
    RUNTIME DESTINATION bin
    BUNDLE DESTINATION .
)
