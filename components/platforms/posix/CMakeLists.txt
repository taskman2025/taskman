cmake_minimum_required(VERSION 3.16)

find_package(PkgConfig REQUIRED)
pkg_check_modules(PROCPS REQUIRED libprocps)

add_library(posix_platform STATIC
    "include/taskman/platform/PosixCommonDefs.h"
    "include/taskman/platform/PosixProcessReader.h"         "src/PosixProcessReader.cpp"
    "include/taskman/platform/PosixProcessFilter.h"         "src/PosixProcessFilter.cpp"
    "include/taskman/platform/PosixPlatform.h"              "src/PosixPlatform.cpp"
)

target_include_directories(posix_platform PUBLIC
    "${CMAKE_CURRENT_LIST_DIR}/include"
    "${PROCPS_INCLUDE_DIRS}"
)

target_link_libraries(posix_platform PUBLIC
    platform_interface
)

link_directories(${PROCPS_LIBRARY_DIRS})

target_link_libraries(posix_platform PUBLIC
    "${PROCPS_LIBRARIES}"
)
