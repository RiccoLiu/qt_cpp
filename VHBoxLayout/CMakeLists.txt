cmake_minimum_required(VERSION 3.19)
project(VHBoxLayout LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets)

qt_standard_project_setup()

qt_add_executable(VHBoxLayout
    WIN32 MACOSX_BUNDLE
    main.cpp
    mainwidget.cpp
    mainwidget.h
    testcls.h testcls.cpp
)

target_link_libraries(VHBoxLayout
    PRIVATE
        Qt::Core
        Qt::Widgets
)

include(GNUInstallDirs)

install(TARGETS VHBoxLayout
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET VHBoxLayout
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
