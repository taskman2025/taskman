cmake_minimum_required(VERSION 3.16)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.9 REQUIRED COMPONENTS Core Concurrent)

add_library(local_posix_backend STATIC
    "include/taskman/backends/LocalPosixBackend.h"    "src/LocalPosixBackend.cpp"
)

target_include_directories(local_posix_backend PUBLIC
    "${CMAKE_CURRENT_LIST_DIR}/include"
)

target_link_libraries(local_posix_backend PUBLIC backends_local_interface)
target_link_libraries(local_posix_backend PUBLIC posix_platform_runtime)

include(GNUInstallDirs)
