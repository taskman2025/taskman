cmake_minimum_required(VERSION 3.16)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 REQUIRED COMPONENTS Widgets Concurrent)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Concurrent)

set(PROJECT_SOURCES
        "src/main.cpp"
        "include/taskman/client/MainWindow.h"                 "src/MainWindow.cpp"
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(taskman
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET taskman APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(taskman SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(taskman
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_include_directories(taskman PRIVATE "${CMAKE_CURRENT_LIST_DIR}/include")
target_link_libraries(taskman PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)



##################################
####### taskman libraries ########

if (UNIX AND NOT APPLE)
    message(STATUS "Configuring for Linux/POSIX")
    target_link_libraries(taskman PUBLIC local_posix)
elseif(APPLE)
    message(STATUS "Configuring for macOS")
    message(FATAL_ERROR "This platform will be supported soon: ${CMAKE_SYSTEM_NAME}")
elseif(WIN32)
    message(STATUS "Configuring for Windows")
    message(FATAL_ERROR "This platform will be supported soon: ${CMAKE_SYSTEM_NAME}")
else()
    message(FATAL_ERROR "Unsupported platform: ${CMAKE_SYSTEM_NAME}")
endif()

##################################



# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.taskman)
endif()
set_target_properties(taskman PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS taskman
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(taskman)
endif()







# --- CPACK CONFIGURATION ---
include(InstallRequiredSystemLibraries)

set(CPACK_PACKAGE_NAME "taskman")
set(CPACK_PACKAGE_VENDOR "Your Name or Org")
set(CPACK_PACKAGE_CONTACT "you@example.com")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "TaskMan - Cross-platform process monitor")
set(CPACK_PACKAGE_VERSION "1.0.0")
set(CPACK_PACKAGE_VERSION_MAJOR "1")
set(CPACK_PACKAGE_VERSION_MINOR "0")
set(CPACK_PACKAGE_VERSION_PATCH "0")

set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Your Name")  # Required for .deb
set(CPACK_RPM_PACKAGE_LICENSE "MIT")              # Or your license
set(CPACK_GENERATOR "DEB;RPM")

include(CPack)

install(FILES ../../taskman.desktop DESTINATION share/applications)
# install(FILES icons/taskman.png DESTINATION share/pixmaps)
