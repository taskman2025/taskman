cmake_minimum_required(VERSION 3.16)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)

add_library(platform_profiles_interface STATIC
    "include/taskman/platform_profiles/ProcessField.h"
    "include/taskman/platform_profiles/ProcessFilterType.h"
    "include/taskman/platform_profiles/ProcessAction.h"
    "include/taskman/platform_profiles/IPlatformProfile.h"      "src/IPlatformProfile.cpp"
)

target_include_directories(platform_profiles_interface PUBLIC
    "${CMAKE_CURRENT_LIST_DIR}/include"
)

target_link_libraries(platform_profiles_interface PUBLIC Qt${QT_VERSION_MAJOR}::Core)
target_link_libraries(platform_profiles_interface PUBLIC common_interface)

include(GNUInstallDirs)
