cmake_minimum_required(VERSION 3.19)
project(mvs LANGUAGES CXX)

# Concurrent
find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets Concurrent)

find_package(OpenCV REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS Widgets OpenGL OpenGLWidgets)

qt_standard_project_setup()

set(CMAKE_AUTORCC ON)   # 自动处理 .qrc 文件
set(CMAKE_AUTOMOC ON)   #
set(CMAKE_AUTOUIC ON)

qt_add_executable(mvs
    WIN32 MACOSX_BUNDLE
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    resource/resources.qrc
    device//device_manager.h
    device/device_manager.cpp
    device/feature_manager.h
    device/feature_manager.cpp
    device/feature_widget_factory.h
    device/feature_widget_factory.cpp
    device_view.h
    device_view.cpp
    image_view.h
    image_view.cpp
    property_view.h
    property_view.cpp
    image_control.h image_control.cpp
    vcpkg.json
    video_worker.h video_worker.cpp
    controls/searchbar.h controls/searchbar.cpp
    controls/imagelabel.h controls/imagelabel.cpp
    controls/circlebutton.h controls/circlebutton.cpp
    controls/ringprogress.h controls/ringprogress.cpp
    controls/timeline_widget.h controls/timeline_widget.cpp
    controls/ringprogress_performance.h controls/ringprogress_performance.cpp

    scannerwindow.h scannerwindow.cpp
    controls/scannerview.h controls/scannerview.cpp
    controls/mygl.h controls/mygl.cpp
)

target_link_libraries(mvs
    PUBLIC
        ${OpenCV_LIBS}
    PRIVATE
        Qt::Core
        Qt::Widgets
        Qt::Concurrent
        Qt6::Widgets
        Qt6::OpenGL
        Qt6::OpenGLWidgets  # ← QOpenGLWidget 在这个模块里
)

include(GNUInstallDirs)

install(TARGETS mvs
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET mvs
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
