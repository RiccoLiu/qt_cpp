cmake_minimum_required(VERSION 3.19)
project(qt_tips LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core)

qt_standard_project_setup()

qt_add_executable(qt_tips
    main.cpp
)

target_link_libraries(qt_tips
    PRIVATE
        Qt::Core
)

include(GNUInstallDirs)

install(TARGETS qt_tips
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
