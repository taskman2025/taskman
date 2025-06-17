cmake_minimum_required(VERSION 3.16)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)

add_library(platform_runtimes_interface STATIC
    "include/taskman/platform_runtimes/IPlatformRuntime.h"      "src/IPlatformRuntime.cpp"
    "include/taskman/platform_runtimes/IProcessReader.h"        "src/IProcessReader.cpp"
    "include/taskman/platform_runtimes/IProcessFilter.h"        "src/IProcessFilter.cpp"
)

target_include_directories(platform_runtimes_interface PUBLIC
    "${CMAKE_CURRENT_LIST_DIR}/include"
)

target_link_libraries(platform_runtimes_interface PUBLIC Qt${QT_VERSION_MAJOR}::Core)
target_link_libraries(platform_runtimes_interface PUBLIC platform_profiles_interface)
target_link_libraries(platform_runtimes_interface PUBLIC common_interface)

include(GNUInstallDirs)
