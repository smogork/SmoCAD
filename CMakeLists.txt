cmake_minimum_required(VERSION 3.5)

project(SmoCAD VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets LinguistTools REQUIRED)
find_package(Qt6 COMPONENTS OpenGLWidgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets LinguistTools REQUIRED)

set(TS_FILES SmoCAD_en_GB.ts)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        glwidget.cpp
        glwidget.h
        ${TS_FILES}
        Objects/TransformableObject.cpp Objects/TransformableObject.h Renderer/Camera/OrbitalCamera.cpp Renderer/Camera/OrbitalCamera.h Objects/TorusObject.cpp Objects/TorusObject.h Objects/IRenderableObject.cpp Objects/IRenderableObject.h)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(SmoCAD
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET SmoCAD APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation

    qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
else()
    if(ANDROID)
        add_library(SmoCAD SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(SmoCAD
            ${PROJECT_SOURCES}
        )
    endif()

    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif()

target_link_libraries(SmoCAD PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(SmoCAD PRIVATE Qt${QT_VERSION_MAJOR}::OpenGLWidgets)

set_target_properties(SmoCAD PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

add_custom_command(
        TARGET SmoCAD POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_SOURCE_DIR}/Shaders/test.vert
        ${CMAKE_CURRENT_BINARY_DIR}/test.vert)
add_custom_command(
        TARGET SmoCAD POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_SOURCE_DIR}/Shaders/test.frag
        ${CMAKE_CURRENT_BINARY_DIR}/test.frag)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(SmoCAD)
endif()
