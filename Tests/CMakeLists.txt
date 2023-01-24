cmake_minimum_required(VERSION 3.12)

#find_package(LAPACK REQUIRED)
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Gui REQUIRED)
find_package(GTest REQUIRED)

include_directories(${CMAKE_SOURCE_DIR}/)


list(TRANSFORM MATH_FILES PREPEND ${CMAKE_SOURCE_DIR}/)

set(TEST_FILES
        InterpolationC2Test.cpp InterpolationC2Test.h
        test_main.cpp
        DeCasteljauTests.cpp DeCasteljauTests.h
        GradientMethodTests.cpp GradientMethodTests.h
        CommonTestClass.cpp CommonTestClass.h
        NewtonRaphsonTests.cpp NewtonRaphsonTests.h
        Units/BaseUnitTests.cpp Units/BaseUnitTests.h
        Units/DistanceTests.cpp Units/DistanceTests.h
        GeomteryRelationsTests.cpp GeomteryRelationsTests.h)

add_executable(SmoCAD_Tests ${TEST_FILES} ${MATH_FILES} ../Scene/Utilities/Utilites.h ../Scene/Utilities/Utilites.cpp)

target_include_directories(SmoCAD_Tests PRIVATE ${PROJECT_BINARY_DIR}/ThirdParty/Scene-Serializer/cpp/Serializer/include/Serializer)
target_link_libraries(SmoCAD_Tests PRIVATE GTest::gtest GTest::gtest_main)
target_link_libraries(SmoCAD_Tests PRIVATE Qt${QT_VERSION_MAJOR}::Core)
target_link_libraries(SmoCAD_Tests PRIVATE Qt${QT_VERSION_MAJOR}::Gui)
target_link_directories(SmoCAD_Tests PRIVATE ${PROJECT_BINARY_DIR}/ThirdParty/Scene-Serializer/cpp/Serializer/)