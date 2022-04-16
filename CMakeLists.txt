cmake_minimum_required(VERSION 3.5)

project(SmoCAD VERSION 0.5 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_definitions(Q_ENABLE_OPENGL_FUNCTIONS_DEBUG)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets LinguistTools REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS OpenGLWidgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Gui REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS 3DCore REQUIRED)

set(TS_FILES SmoCAD_en_GB.ts)
set(SHADER_FILES
        Shaders/buffer_color.vert
        Shaders/uniform_color.vert
        Shaders/simple_color.frag
        Shaders/bezier.vert
        Shaders/bezier.tess
        Shaders/bezier.eval
        Shaders/bezier.frag
        )

set(CONTROL_SOURCES
        mainwindow.cpp mainwindow.h mainwindow.ui
        glwidget.cpp glwidget.h
        Controls/transformcontrol.h Controls/transformcontrol.cpp Controls/transformcontrol.ui
        Controls/uvcontrol.h Controls/uvcontrol.cpp Controls/uvcontrol.ui
        Controls/cursorcontrol.h Controls/cursorcontrol.cpp Controls/cursorcontrol.ui
        Controls/transformcollectioncontrol.h Controls/transformcollectioncontrol.cpp Controls/transformcollectioncontrol.ui
        Controls/sceneelementslist.h Controls/sceneelementslist.cpp Controls/sceneelementslist.ui
    )

set(PROJECT_SOURCES
        main.cpp
        ${CONTROL_SOURCES}
        ${TS_FILES}
        Objects/TransformableObject.cpp Objects/TransformableObject.h
        Renderer/Camera/OrbitalCamera.cpp Renderer/Camera/OrbitalCamera.h
        Objects/TorusObject.cpp Objects/TorusObject.h
        Objects/IRenderableObject.cpp Objects/IRenderableObject.h
        Objects/CubeObject.cpp Objects/CubeObject.h
        Renderer/ShaderWrapper.cpp Renderer/ShaderWrapper.h
        Renderer/InputController/InputController.cpp Renderer/InputController/InputController.h
        Renderer/InputController/InputEvents/CameraUpdateEvent.cpp Renderer/InputController/InputEvents/CameraUpdateEvent.h
        Objects/CursorObject.cpp Objects/CursorObject.h
        Renderer/InputController/InputEvents/SceneMouseClickEvent.cpp Renderer/InputController/InputEvents/SceneMouseClickEvent.h
        Renderer/Camera/Viewport.cpp Renderer/Camera/Viewport.h
        Objects/PointObject.cpp Objects/PointObject.h
        Scene/SceneModelOld.cpp Scene/SceneModelOld.h
        Scene/Events/CursorChangedEvent.cpp Scene/Events/CursorChangedEvent.h
        Scene/Events/SelectedObjectChangedEvent.cpp Scene/Events/SelectedObjectChangedEvent.h
        Objects/CompositeObject.cpp Objects/CompositeObject.h
        Objects/BezierCurveC0.cpp Objects/BezierCurveC0.h
        Scene/Events/PointObjectChangedEvent.cpp Scene/Events/PointObjectChangedEvent.h
        Scene/Systems/ISystem.cpp Scene/Systems/ISystem.h
        Scene/Entities/IEntity.cpp Scene/Entities/IEntity.h
        Scene/Components/IComponent.cpp Scene/Components/IComponent.h
        Scene/Entities/Point.cpp Scene/Entities/Point.h
        Scene/Components/Transform.cpp Scene/Components/Transform.h
        Scene/SceneECS.cpp Scene/SceneECS.h
        Scene/Systems/TransformSystem.cpp Scene/Systems/TransformSystem.h
        Scene/Utilities/TypeMap.cpp Scene/Utilities/TypeMap.h
        Scene/Utilities/TypeNumbering.cpp Scene/Utilities/TypeNumbering.h
        Scene/Components/Drawing/Drawing.cpp Scene/Components/Drawing/Drawing.h
        Scene/Systems/DrawingSystem.cpp Scene/Systems/DrawingSystem.h
        Scene/Systems/IAbstractSystem.cpp Scene/Systems/IAbstractSystem.h
        Scene/Entities/Cube.cpp Scene/Entities/Cube.h
        Renderer/BufferLayout.cpp Renderer/BufferLayout.h
        Renderer/Renderer.cpp Renderer/Renderer.h
        Scene/Entities/Grid.cpp Scene/Entities/Grid.h
        Scene/Components/Drawing/DynamicDrawing.cpp Scene/Components/Drawing/DynamicDrawing.h
        Scene/Components/Drawing/StaticDrawing.cpp Scene/Components/Drawing/StaticDrawing.h
        Scene/Entities/Torus.cpp Scene/Entities/Torus.h
        Scene/Components/UVParams.cpp Scene/Components/UVParams.h
        Scene/Systems/UVParamsSystem.cpp Scene/Systems/UVParamsSystem.h
        Scene/Events/SystemEvents/UVParamsChanged.cpp Scene/Events/SystemEvents/UVParamsChanged.h
        Scene/Entities/Cursor.cpp Scene/Entities/Cursor.h
        Scene/Systems/SelectableSystem.cpp Scene/Systems/SelectableSystem.h
        Scene/Components/Selectable.cpp Scene/Components/Selectable.h
        Scene/Events/SystemEvents/SelectionChanged.cpp Scene/Events/SystemEvents/SelectionChanged.h
        Scene/Entities/Composite.cpp Scene/Entities/Composite.h
        Scene/Components/CompositeAware.cpp Scene/Components/CompositeAware.h
        Scene/Systems/CompositeAwareSystem.cpp Scene/Systems/CompositeAwareSystem.h
        Scene/Entities/Polyline.cpp Scene/Entities/Polyline.h
        Scene/Components/TransformCollection.cpp Scene/Components/TransformCollection.h
        Scene/Components/CollectionAware.cpp Scene/Components/CollectionAware.h
        Scene/Systems/TransformCollectionSystem.cpp Scene/Systems/TransformCollectionSystem.h
        Scene/Systems/CollectionAwareSystem.cpp Scene/Systems/CollectionAwareSystem.h
        Scene/Utilities/Utilites.cpp Scene/Utilities/Utilites.h
        Scene/Entities/VirtualBezierC0.cpp Scene/Entities/VirtualBezierC0.h
        Scene/Entities/BezierC2.cpp Scene/Entities/BezierC2.h
        Scene/Entities/VirtualPoint.cpp Scene/Entities/VirtualPoint.h Controls/ComponentControl.cpp Controls/ComponentControl.h Scene/Components/SceneElement.cpp Scene/Components/SceneElement.h Scene/Components/ScreenSelectable.cpp Scene/Components/ScreenSelectable.h Scene/Systems/ScreenSelectableSystem.cpp Scene/Systems/ScreenSelectableSystem.h Scene/Systems/SceneElementSystem.cpp Scene/Systems/SceneElementSystem.h Scene/Entities/BezierC0.cpp Scene/Entities/BezierC0.h)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(SmoCAD
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET SmoCAD APPEND PROPsERTY QT_ANDROID_PACKAGE_SOURCE_DIR
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
target_link_libraries(SmoCAD PRIVATE Qt${QT_VERSION_MAJOR}::Gui)
target_link_libraries(SmoCAD PRIVATE Qt${QT_VERSION_MAJOR}::3DCore)
target_link_libraries(SmoCAD PRIVATE Qt${QT_VERSION_MAJOR}::Core)

set_target_properties(SmoCAD PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

# Copy shaders
foreach( shader_file ${SHADER_FILES})
    add_custom_command(
            TARGET SmoCAD POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_SOURCE_DIR}/${shader_file}
            ${CMAKE_CURRENT_BINARY_DIR}/${shader_file})
endforeach( shader_file )

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(SmoCAD)
endif()
