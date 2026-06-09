cmake_minimum_required(VERSION 3.19)
project(mysql LANGUAGES C CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets)
find_package(OpenCV REQUIRED)

qt_standard_project_setup()

qt_add_executable(mysql
    WIN32 MACOSX_BUNDLE
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    usermanager_window.h usermanager_window.cpp usermanager_window.ui
    userinfo_dlg.h userinfo_dlg.cpp userinfo_dlg.ui
    management_window.h management_window.cpp management_window.ui
    videodecoder.h videodecoder.cpp
    facedetector.h facedetector.cpp
    utils.h
)

target_link_libraries(mysql
PRIVATE
    Qt::Core
    Qt::Widgets
    # database
    logger2
    database2
    ${OpenCV_LIBS}
)

include(GNUInstallDirs)

install(TARGETS mysql
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET mysql
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})

# add_subdirectory(logger)
add_subdirectory(logger2)
add_subdirectory(utils)
add_subdirectory(test)
add_subdirectory(database2)
add_subdirectory(pipeline)
add_subdirectory(pipeline2)
