cmake_minimum_required(VERSION 3.16)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 REQUIRED COMPONENTS Core Gui)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Gui)

add_library(platform_interface STATIC
    "include/taskman/platform/ProcessField.h"       "src/ProcessField.cpp"
    "include/taskman/platform/IProcessReader.h"     "src/IProcessReader.cpp"
    "include/taskman/platform/IProcessFilter.h"     "src/IProcessFilter.cpp"
    "include/taskman/platform/IPlatform.h"          "src/IPlatform.cpp"
)

target_include_directories(platform_interface PUBLIC
    "${CMAKE_CURRENT_LIST_DIR}/include"
)

target_link_libraries(platform_interface PUBLIC Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Gui)
target_link_libraries(platform_interface PUBLIC common_interface)

include(GNUInstallDirs)
