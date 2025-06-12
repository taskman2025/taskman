cmake_minimum_required(VERSION 3.16)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Concurrent)

add_library(backend_interface STATIC
    "include/taskman/backend/ProcessData.h"                 "src/ProcessData.cpp"
    "include/taskman/backend/IBackend.h"                    "src/IBackend.cpp"
    "include/taskman/backend/IProcessItemModel.h"           "src/IProcessItemModel.cpp"
    "include/taskman/backend/IConnectionTab.h"              "src/IConnectionTab.cpp"
    "include/taskman/backend/ISortFilterProxyModel.h"       "src/ISortFilterProxyModel.cpp"
)

target_include_directories(backend_interface PUBLIC
    "${CMAKE_CURRENT_LIST_DIR}/include"
)

target_link_libraries(backend_interface PUBLIC Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Concurrent)
target_link_libraries(backend_interface PUBLIC common_interface)
target_link_libraries(backend_interface PUBLIC platform_interface)

include(GNUInstallDirs)
