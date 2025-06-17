cmake_minimum_required(VERSION 3.16)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 REQUIRED COMPONENTS Core Concurrent)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Concurrent)

add_library(common_interface STATIC
    # "include/taskman/common/ThreadsafeQueue.h"
    "include/taskman/common/ThreadsafeSharedConstPointer.h"
    "include/taskman/common/ThreadsafeConstReadProxy.h"
    "include/taskman/common/types.h"
    "include/taskman/common/type_traits.h"
    "include/taskman/common/bit_ops.h"
    "include/taskman/common/declaration.h"
    "include/taskman/common/FilterParamType.h"
    "include/taskman/common/IntervalUpdateReminder.h"   "src/IntervalUpdateReminder.cpp"
    "include/taskman/common/concurrent.h"               "src/concurrent.cpp"
)

target_include_directories(common_interface PUBLIC
    "${CMAKE_CURRENT_LIST_DIR}/include"
)

target_link_libraries(common_interface PUBLIC Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Concurrent)

include(GNUInstallDirs)
