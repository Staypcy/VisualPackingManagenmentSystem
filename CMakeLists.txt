cmake_minimum_required(VERSION 3.16)

project(VisualParkingManagementSystem VERSION 0.2 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Sql Test)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Sql Test)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

# Collect all source files
file(GLOB_RECURSE ALL_SOURCES CONFIGURE_DEPENDS
    src/*.cpp
    src/*.h
    src/*.ui
)
list(FILTER ALL_SOURCES EXCLUDE REGEX "old_unused/")

# Separate main.cpp from library sources
set(LIB_SOURCES ${ALL_SOURCES})
list(FILTER LIB_SOURCES EXCLUDE REGEX "main\\.cpp$")

# Static library for core code (shared between app and tests)
add_library(vpms_lib STATIC ${LIB_SOURCES})
target_link_libraries(vpms_lib PUBLIC
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Sql
)

# Main executable
if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(VisualParkingManagementSystem
        MANUAL_FINALIZATION
        src/main.cpp
    )
else()
    add_executable(VisualParkingManagementSystem
        src/main.cpp
    )
endif()

target_link_libraries(VisualParkingManagementSystem PRIVATE vpms_lib)

if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.VisualParkingManagementSystem)
endif()
set_target_properties(VisualParkingManagementSystem PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS VisualParkingManagementSystem
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(VisualParkingManagementSystem)
endif()

enable_testing()
add_subdirectory(tests)
