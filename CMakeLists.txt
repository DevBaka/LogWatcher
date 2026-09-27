cmake_minimum_required(VERSION 3.20)
project(LogWatcher VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Add required compiler flag for MSVC
if(MSVC)
    add_compile_options(/Zc:__cplusplus)
endif()

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Qt6 configuration
# Set CMAKE_PREFIX_PATH to Qt6 installation directory if not already set
if(NOT DEFINED CMAKE_PREFIX_PATH AND DEFINED ENV{Qt6_DIR})
    set(CMAKE_PREFIX_PATH $ENV{Qt6_DIR})
endif()

# Common Qt6 installation paths on Windows
if(WIN32 AND NOT DEFINED CMAKE_PREFIX_PATH)
    set(POSSIBLE_QT_PATHS
        "C:/Qt/6.*/msvc2019_64"
        "C:/Qt/6.*/msvc2022_64"
        "$ENV{PROGRAMFILES}/Qt/6.*/msvc2019_64"
        "$ENV{PROGRAMFILES}/Qt/6.*/msvc2022_64"
    )
    
    foreach(PATH ${POSSIBLE_QT_PATHS})
        file(GLOB QT_PATH ${PATH})
        if(QT_PATH)
            set(CMAKE_PREFIX_PATH ${QT_PATH})
            message(STATUS "Found Qt6 at: ${QT_PATH}")
            break()
        endif()
    endforeach()
endif()

# Check for MinGW Qt with MSVC compiler (incompatible)
if(MSVC AND CMAKE_PREFIX_PATH MATCHES "mingw")
    message(FATAL_ERROR "You are using MinGW version of Qt6 with MSVC compiler. "
        "Please install the MSVC version of Qt6 (e.g., msvc2022_64) instead of mingw_64. "
        "Current Qt path: ${CMAKE_PREFIX_PATH}")
endif()

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

set(PROJECT_SOURCES
    src/main.cpp
    src/MainWindow.cpp
    src/LogViewerWidget.cpp
    src/FileTracker.cpp
    src/ProjectExplorer.cpp
)

set(PROJECT_HEADERS
    src/MainWindow.h
    src/LogViewerWidget.h
    src/FileTracker.h
    src/ProjectExplorer.h
)

add_executable(LogWatcher
    ${PROJECT_SOURCES}
    ${PROJECT_HEADERS}
)

target_link_libraries(LogWatcher PRIVATE
    Qt6::Core
    Qt6::Widgets
)

if(WIN32)
    set_target_properties(LogWatcher PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
    
    # Copy Qt6 DLLs to output directory automatically
    get_target_property(QT6_QMAKE_EXECUTABLE Qt6::Core INTERFACE_QT_EXECUTABLE)
    if(QT6_QMAKE_EXECUTABLE)
        get_filename_component(QT6_BIN_DIR "${QT6_QMAKE_EXECUTABLE}" DIRECTORY)
        get_filename_component(QT6_ROOT_DIR "${QT6_BIN_DIR}" DIRECTORY)
        
        # Copy required Qt6 DLLs
        add_custom_command(TARGET LogWatcher POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${QT6_ROOT_DIR}/bin/Qt6Core$<$<CONFIG:Debug>:d>.dll"
                "${QT6_ROOT_DIR}/bin/Qt6Gui$<$<CONFIG:Debug>:d>.dll"
                "${QT6_ROOT_DIR}/bin/Qt6Widgets$<$<CONFIG:Debug>:d>.dll"
                $<TARGET_FILE_DIR:LogWatcher>
            COMMENT "Copying Qt6 DLLs to output directory"
        )
    endif()
endif()

install(TARGETS LogWatcher
    RUNTIME DESTINATION bin
)
