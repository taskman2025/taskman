cmake_minimum_required(VERSION 3.16)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)

add_library(posix_platform_profile STATIC
    "include/taskman/platform_profiles/posix/process_field_bits.h"
    "include/taskman/platform_profiles/posix/process_filter_type_ids.h"
    "include/taskman/platform_profiles/posix/process_action_ids.h"
    "include/taskman/platform_profiles/posix/constants.h"
    "include/taskman/platform_profiles/PosixPlatformProfile.h"          "src/PosixPlatformProfile.cpp"
)

target_include_directories(posix_platform_profile PUBLIC
    "${CMAKE_CURRENT_LIST_DIR}/include"
)

target_link_libraries(posix_platform_profile PUBLIC Qt${QT_VERSION_MAJOR}::Core)
target_link_libraries(posix_platform_profile PUBLIC platform_profiles_interface)

include(GNUInstallDirs)
