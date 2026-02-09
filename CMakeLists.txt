cmake_minimum_required(VERSION 3.25)

set(PROJECT_NAME "DiagramPche_Qt")
set(BINARY_NAME "DiagramPche_Qt")

# == Project setup ==
project("${PROJECT_NAME}")
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_STANDARD 20)

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
find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

# == Executable setup ==
file(GLOB_RECURSE SOURCES
        "${PROJECT_SOURCE_DIR}/App/*.cpp"
)
add_executable("${BINARY_NAME}"
        "Run.cpp"
        ${SOURCES}
)

# == Link libraries ==
target_link_libraries("${BINARY_NAME}" Qt6::Widgets)

# == Misc ==
if (MSVC)
    target_compile_definitions("${CMAKE_PROJECT_NAME}" PUBLIC _CRT_SECURE_NO_WARNINGS)
    #set_target_properties("${CMAKE_PROJECT_NAME}" PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup") # No console
endif ()
