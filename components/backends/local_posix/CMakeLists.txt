cmake_minimum_required(VERSION 3.16)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(local_posix STATIC
    "include/taskman/backend/local_posix/PosixProcessItemModel.h"   "src/PosixProcessItemModel.cpp"
    "include/taskman/backend/local_posix/LocalPosixConnectionTab.h" "src/LocalPosixConnectionTab.cpp"
    "include/taskman/backend/local_posix/LocalPosixBackend.h"       "src/LocalPosixBackend.cpp"
)

target_include_directories(local_posix PUBLIC
    "${CMAKE_CURRENT_LIST_DIR}/include"
)

target_link_libraries(local_posix PUBLIC backend_interface)
target_link_libraries(local_posix PUBLIC posix_platform)
target_link_libraries(local_posix PUBLIC common_ui)
