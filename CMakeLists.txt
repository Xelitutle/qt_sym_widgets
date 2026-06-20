cmake_minimum_required(VERSION 3.10)

project(qt_sym_widgets 
        VERSION 1.0
        LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)

find_package(Qt5 COMPONENTS Widgets Core REQUIRED)

set(SOURCES
    sources/main.cpp
    sources/symwidgets/SymWidget.cpp
    sources/symwidgets/SymMainWidget.cpp
    sources/symwidgets/SymFrame.cpp
    sources/symwidgets/SymLabel.cpp
    sources/symwidgets/SymButton.cpp
    sources/symwidgets/SymCheckBox.cpp
    sources/symwidgets/SymIntInput.cpp
    sources/symwidgets/SymFloatInput.cpp
)

set(HEADERS
    include/symwidgets/GridMetrics.h
    include/symwidgets/Theme.h
    include/symwidgets/SymWidget.h
    include/symwidgets/SymMainWidget.h
    include/symwidgets/SymFrame.h
    include/symwidgets/SymLabel.h
    include/symwidgets/SymButton.h
    include/symwidgets/SymCheckBox.h
    include/symwidgets/SymIntInput.h
    include/symwidgets/SymFloatInput.h
)

add_executable(${PROJECT_NAME} WIN32
    ${SOURCES}
    ${HEADERS}
)

target_include_directories(${PROJECT_NAME}
    PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(${PROJECT_NAME}
    Qt5::Widgets
    Qt5::Core
)

target_compile_definitions(${PROJECT_NAME}
    PRIVATE
    ETC_DIR="${CMAKE_SOURCE_DIR}/etc"
)

if (MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /utf-8)
endif()
