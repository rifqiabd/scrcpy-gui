cmake_minimum_required(VERSION 3.16)
project(scrcpy-gui VERSION 1.0.8 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
# CMAKE_AUTOUIC removed - QML doesn't use .ui files

# Find Qt6 - Added Quick, Qml, QuickControls2 for QML support
find_package(Qt6 6.8 REQUIRED COMPONENTS Core Widgets Gui Quick Qml QuickControls2)

# Source files
set(SOURCES
    src/main.cpp
    src/mainwindow.cpp
    src/appmanager.cpp
    src/settingsdialog.cpp
    src/applistmodel.cpp
    src/settingsmanager.cpp
)

set(HEADERS
    src/mainwindow.h
    src/appmanager.h
    src/settingsdialog.h
    src/applistmodel.h
    src/settingsmanager.h
)

# Resource files
set(RESOURCES
    resources.qrc
    qml.qrc
)

# Create executable
add_executable(${PROJECT_NAME}
    MACOSX_BUNDLE
    ${SOURCES}
    ${HEADERS}
    ${RESOURCES}
)

# Link Qt libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::Gui
    Qt6::Quick
    Qt6::Qml
    Qt6::QuickControls2
)

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Platform-specific settings
if(WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
endif()

# Install rules
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
    BUNDLE DESTINATION .
)
