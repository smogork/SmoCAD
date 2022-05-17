cmake_minimum_required(VERSION 3.12)

project(SmoCAD VERSION 0.5 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_definitions(Q_ENABLE_OPENGL_FUNCTIONS_DEBUG)

find_package(LAPACK REQUIRED)

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
        Shaders/plain.vert
        Shaders/plain.tess
        Shaders/plain.eval
        Shaders/plain.frag
        )

set(MATH_FILES
        Mathematics/Interpolation.cpp Mathematics/Interpolation.h
        Mathematics/clapack.h
        Mathematics/f2c.h
        )

set(CONTROL_SOURCES
        mainwindow.cpp mainwindow.h mainwindow.ui
        glwidget.cpp glwidget.h
        Controls/ComponentControl.cpp Controls/ComponentControl.h
        Controls/transformcontrol.h Controls/transformcontrol.cpp Controls/transformcontrol.ui
        Controls/uvcontrol.h Controls/uvcontrol.cpp Controls/uvcontrol.ui
        Controls/cursorcontrol.h Controls/cursorcontrol.cpp Controls/cursorcontrol.ui
        Controls/transformcollectioncontrol.h Controls/transformcollectioncontrol.cpp Controls/transformcollectioncontrol.ui
        Controls/sceneelementslist.h Controls/sceneelementslist.cpp Controls/sceneelementslist.ui
        Controls/ListElements/QListWidgetSceneElement.cpp Controls/ListElements/QListWidgetSceneElement.h
        Controls/ListElements/QListWidgetCollectionElement.cpp Controls/ListElements/QListWidgetCollectionElement.h
        Controls/stereoscopicconfig.h Controls/stereoscopicconfig.cpp Controls/stereoscopicconfig.ui
    )

set(PROJECT_SOURCES
        main.cpp
        ${CONTROL_SOURCES}
        ${TS_FILES}
        ${MATH_FILES}
        Renderer/Camera/OrbitalCamera.cpp Renderer/Camera/OrbitalCamera.h
        Renderer/ShaderWrapper.cpp Renderer/ShaderWrapper.h
        Renderer/InputController/InputController.cpp Renderer/InputController/InputController.h
        Renderer/InputController/InputEvents/CameraUpdateEvent.cpp Renderer/InputController/InputEvents/CameraUpdateEvent.h
        Renderer/InputController/InputEvents/SceneMouseClickEvent.cpp Renderer/InputController/InputEvents/SceneMouseClickEvent.h
        Renderer/Camera/Viewport.cpp Renderer/Camera/Viewport.h
        Scene/Events/CursorChangedEvent.cpp Scene/Events/CursorChangedEvent.h
        Scene/Systems/ISystem.cpp Scene/Systems/ISystem.h
        Scene/Entities/IEntity.cpp Scene/Entities/IEntity.h
        Scene/Components/IComponent.cpp Scene/Components/IComponent.h
        Scene/Entities/Points/Point.cpp Scene/Entities/Points/Point.h
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
        Scene/Entities/Curves/VirtualBezierC0.cpp Scene/Entities/Curves/VirtualBezierC0.h
        Scene/Entities/Curves/BezierC2.cpp Scene/Entities/Curves/BezierC2.h
        Scene/Entities/Points/VirtualPoint.cpp Scene/Entities/Points/VirtualPoint.h
        Scene/Components/SceneElement.cpp Scene/Components/SceneElement.h
        Scene/Components/ScreenSelectable.cpp Scene/Components/ScreenSelectable.h
        Scene/Systems/ScreenSelectableSystem.cpp Scene/Systems/ScreenSelectableSystem.h
        Scene/Systems/SceneElementSystem.cpp Scene/Systems/SceneElementSystem.h
        Scene/Entities/Curves/BezierC0.cpp Scene/Entities/Curves/BezierC0.h
        Renderer/Options.cpp Renderer/Options.h
        Scene/Entities/Curves/BernsteinCurve.cpp Scene/Entities/Curves/BernsteinCurve.h
        Scene/Entities/Curves/InterpolationC2.cpp Scene/Entities/Curves/InterpolationC2.h
        Scene/Entities/Points/InvisiblePoint.cpp Scene/Entities/Points/InvisiblePoint.h
        Renderer/InputController/InputEvents/ObjectMoveEvent.cpp Renderer/InputController/InputEvents/ObjectMoveEvent.h
        Controls/CustomQListWidget.cpp Controls/CustomQListWidget.h
        Renderer/Camera/StereoscopicParameters.cpp Renderer/Camera/StereoscopicParameters.h
        Scene/Entities/Plains/PlainC0.cpp Scene/Entities/Plains/PlainC0.h
        Scene/Entities/Mesh.cpp Scene/Entities/Mesh.h Scene/Entities/Plains/PlaneCreator.cpp Scene/Entities/Plains/PlaneCreator.h Scene/Systems/UvPlaneCreatorSystem.cpp Scene/Systems/UvPlaneCreatorSystem.h Scene/Components/UvPlainCreator.cpp Scene/Components/UvPlainCreator.h)

add_subdirectory(Tests)

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

target_link_libraries(SmoCAD PRIVATE lapack blas)
target_link_libraries(SmoCAD PRIVATE m)

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
