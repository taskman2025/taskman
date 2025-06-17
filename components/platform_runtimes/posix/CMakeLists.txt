cmake_minimum_required(VERSION 3.16)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)

find_package(PkgConfig REQUIRED)
pkg_check_modules(PROCPS REQUIRED libprocps)

add_library(posix_platform_runtime STATIC
    "include/taskman/platform_runtimes/PosixPlatformRuntime.h"          "src/PosixPlatformRuntime.cpp"
    "include/taskman/platform_runtimes/posix/PosixProcessReader.h"      "src/PosixProcessReader.cpp"
    "include/taskman/platform_runtimes/posix/PosixActionRunner.h"       "src/PosixActionRunner.cpp"
    "include/taskman/platform_runtimes/posix/process_filters/PosixBaseProcessFilter.h"          "src/PosixBaseProcessFilter.cpp"
    "include/taskman/platform_runtimes/posix/process_filters/PosixOpenFileProcessFilter.h"      "src/PosixOpenFileProcessFilter.cpp"
    "include/taskman/platform_runtimes/posix/PosixProcessFilter.h"                              "src/PosixProcessFilter.cpp"
)

target_include_directories(posix_platform_runtime PUBLIC
    "${CMAKE_CURRENT_LIST_DIR}/include"
    "${PROCPS_INCLUDE_DIRS}"
)

target_link_directories(posix_platform_runtime PUBLIC
    "${PROCPS_LIBRARY_DIRS}"
)

target_link_libraries(posix_platform_runtime PUBLIC Qt${QT_VERSION_MAJOR}::Core)
target_link_libraries(posix_platform_runtime PUBLIC posix_platform_profile)
target_link_libraries(posix_platform_runtime PUBLIC platform_runtimes_interface)
target_link_libraries(posix_platform_runtime PUBLIC ${PROCPS_LIBRARIES})

include(GNUInstallDirs)
