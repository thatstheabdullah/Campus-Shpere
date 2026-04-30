cmake_minimum_required(VERSION 3.16)

project(CampusSphere VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Sql)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Sql)

set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
)

# ── OOP Model headers (included for project structure, not compiled separately)
set(OOP_HEADERS
    models/Content.h
    models/User.h
    models/Post.h
    models/Comment.h
    models/Message.h
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(CampusSphere
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        ${OOP_HEADERS}
    )
else()
    if(ANDROID)
        add_library(CampusSphere SHARED ${PROJECT_SOURCES})
    else()
        add_executable(CampusSphere ${PROJECT_SOURCES})
    endif()
endif()

target_link_libraries(CampusSphere PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Sql
)

set_target_properties(CampusSphere PROPERTIES
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS CampusSphere
    BUNDLE  DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(CampusSphere)
endif()
