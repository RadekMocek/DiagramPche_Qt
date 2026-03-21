cmake_minimum_required(VERSION 3.25)

set(PROJECT_NAME "DiagramPche_Qt")
set(BINARY_NAME "DiagramPche_Qt")

# == Project setup ==
project("${PROJECT_NAME}")
#set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_STANDARD 20)

# == Non-Qt deps ==
add_subdirectory(Dependency/QtAwesome7)
set(QCORO_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(QCORO_WITH_QTDBUS OFF CACHE BOOL "" FORCE)
set(QCORO_WITH_QTNETWORK OFF CACHE BOOL "" FORCE)
set(QCORO_WITH_QTWEBSOCKETS OFF CACHE BOOL "" FORCE)
set(QCORO_DISABLE_DEPRECATED_TASK_H ON CACHE BOOL "" FORCE)
set(QCORO_BUILD_TESTING OFF CACHE BOOL "" FORCE)
set(QCORO_WITH_QTQUICK OFF CACHE BOOL "" FORCE)
set(QCORO_WITH_QML OFF CACHE BOOL "" FORCE)
set(QCORO_WITH_QTTEST OFF CACHE BOOL "" FORCE)
add_subdirectory(Dependency/qcoro-0.13.0)
qcoro_enable_coroutines()

# == Qt setup ==
# What is QTDIR ?
# Windows:
# - QTDIR is a system environment variable pointing to `<path_to_qt_installation>\Qt\6.10.0\msvc2022_64`
# - Two values must be added to system environment variable PATH: `%QTDIR%\bin` & `%QTDIR%\lib`
# Linux:
# - export QTDIR=<path_to_qt_installation>/6.10.0/gcc_64/
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_PREFIX_PATH $ENV{QTDIR})
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Svg)

# == Executable setup ==
file(GLOB_RECURSE SOURCES
        "${PROJECT_SOURCE_DIR}/App/*.cpp"
)
add_executable("${BINARY_NAME}"
        "Run.cpp"
        ${SOURCES}
)

# == Link libraries ==
target_link_libraries("${BINARY_NAME}"
        Qt6::Widgets
        Qt6::Svg
        QtAwesome
        QCoro6::Core
)

# == Misc ==
if (MSVC)
    target_compile_definitions("${CMAKE_PROJECT_NAME}" PUBLIC _CRT_SECURE_NO_WARNINGS)
    target_compile_definitions("${CMAKE_PROJECT_NAME}" PUBLIC NOMINMAX)
    #set_target_properties("${CMAKE_PROJECT_NAME}" PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup") # No console
endif ()

# == Resources ==
# Copy resources folder
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/Resource
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
)
