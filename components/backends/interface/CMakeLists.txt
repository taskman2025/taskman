cmake_minimum_required(VERSION 3.16)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.9 REQUIRED COMPONENTS Core Concurrent)

add_library(backends_interface STATIC
    "include/taskman/backends/IBackend.h"             "src/IBackend.cpp"
    "include/taskman/backends/ProcessData.h"          "src/ProcessData.cpp"
    "include/taskman/backends/ProcessTree.h"
    "include/taskman/backends/ProcessTreeBuilder.h"   "src/ProcessTreeBuilder.cpp"
    "include/taskman/backends/SystemInformation.h"
)

target_include_directories(backends_interface PUBLIC
    "${CMAKE_CURRENT_LIST_DIR}/include"
)

target_link_libraries(backends_interface PUBLIC Qt6::Core Qt6::Concurrent)
target_link_libraries(backends_interface PUBLIC common_interface)
target_link_libraries(backends_interface PUBLIC platform_profiles_interface)
target_link_libraries(backends_interface PUBLIC platform_runtimes_interface)

include(GNUInstallDirs)
