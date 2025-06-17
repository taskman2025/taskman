cmake_minimum_required(VERSION 3.16)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.9 REQUIRED COMPONENTS Core Gui Widgets)

add_library(frontends_base STATIC
    "include/taskman/frontends/BaseProcessItemModel.h"       "src/BaseProcessItemModel.cpp"
    "include/taskman/frontends/BaseConnectionTab.h"          "src/BaseConnectionTab.cpp"
    "include/taskman/frontends/BaseSortFilterProxyModel.h"   "src/BaseSortFilterProxyModel.cpp"
    "include/taskman/frontends/BaseFilterPopup.h"            "src/BaseFilterPopup.cpp"
    "include/taskman/frontends/FrontendProcessFilter.h"      "src/FrontendProcessFilter.cpp"
)

target_include_directories(frontends_base PUBLIC
    "${CMAKE_CURRENT_LIST_DIR}/include"
)

target_link_libraries(frontends_base PUBLIC Qt6::Core Qt6::Gui Qt6::Widgets)
target_link_libraries(frontends_base PUBLIC connections_interface)
target_link_libraries(frontends_base PUBLIC common_ui)

include(GNUInstallDirs)
