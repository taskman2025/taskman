cmake_minimum_required(VERSION 3.16)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.9 REQUIRED COMPONENTS Core Gui Widgets)

add_library(posix_frontend STATIC
    "include/taskman/frontends/PosixProcessItemModel.h"       "src/PosixProcessItemModel.cpp"
)

target_include_directories(posix_frontend PUBLIC
    "${CMAKE_CURRENT_LIST_DIR}/include"
)

target_link_libraries(posix_frontend PUBLIC Qt6::Core Qt6::Gui Qt6::Widgets)
target_link_libraries(posix_frontend PUBLIC frontends_base)
target_link_libraries(posix_frontend PUBLIC posix_platform_profile)

include(GNUInstallDirs)
