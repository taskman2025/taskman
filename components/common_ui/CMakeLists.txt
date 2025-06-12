cmake_minimum_required(VERSION 3.16)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

add_library(common_ui STATIC
    "include/taskman/common_ui/FileInputWidget.h"                 "src/FileInputWidget.cpp"
)

target_include_directories(common_ui PUBLIC
    "${CMAKE_CURRENT_LIST_DIR}/include"
)

target_link_libraries(common_ui PUBLIC Qt6::Core Qt6::Gui Qt6::Widgets)
target_link_libraries(common_ui PUBLIC common_interface)

include(GNUInstallDirs)
