cmake_minimum_required(VERSION 3.12)

find_package(LAPACK REQUIRED)
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Gui REQUIRED)
include_directories(${CMAKE_SOURCE_DIR}/)

list(TRANSFORM MATH_FILES PREPEND ${CMAKE_SOURCE_DIR}/)

set(TEST_FILES
        InterpolationC2Test.cpp InterpolationC2Test.h
        test_main.cpp
        DeCasteljauTests.cpp DeCasteljauTests.h
        GradientMethodTests.cpp GradientMethodTests.h
        CommonTestClass.cpp CommonTestClass.h)

add_executable(SmoCAD_Tests ${TEST_FILES} ${MATH_FILES})

target_link_libraries(SmoCAD_Tests PRIVATE gtest)
target_link_libraries(SmoCAD_Tests PRIVATE Qt${QT_VERSION_MAJOR}::Core)
target_link_libraries(SmoCAD_Tests PRIVATE Qt${QT_VERSION_MAJOR}::Gui)
target_link_libraries(SmoCAD_Tests PRIVATE lapack blas)