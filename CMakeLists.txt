cmake_minimum_required(VERSION 3.5)

project(ElementScanner VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5  COMPONENTS Widgets  REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets Charts REQUIRED)


set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        analyzer.cpp
        analyzer.hpp
        element.cpp
        element.hpp
        prim_exception.hpp
        tools.cpp
        tools.hpp
        console.h console.cpp console.ui
        autoanalysisconfig.h autoanalysisconfig.cpp autoanalysisconfig.ui
        manualanalysisconfig.h manualanalysisconfig.cpp manualanalysisconfig.ui
        Resources.qrc
        mygraphicsview.h mygraphicsview.cpp
        configwindow.h configwindow.cpp configwindow.ui
        currentmaskanalysisconfig.h currentmaskanalysisconfig.cpp currentmaskanalysisconfig.ui
        fullrangeanalysisconfig.h fullrangeanalysisconfig.cpp fullrangeanalysisconfig.ui
        thresholdanalysisconfig.h thresholdanalysisconfig.cpp thresholdanalysisconfig.ui
        chartwindow.h chartwindow.cpp chartwindow.ui
        AppIconResFile.qrc
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(ElementScanner
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET QElementAnalysis APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(ElementScanner SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(ElementScanner
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(ElementScanner PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Charts)

set_target_properties(ElementScanner PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(ElementScanner)
endif()
