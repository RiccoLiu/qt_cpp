cmake_minimum_required(VERSION 3.16)

project(test LANGUAGES C CXX)

# set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

message("CMAKE_SOURCE_DIR = ${CMAKE_SOURCE_DIR}")

add_executable(test
    main.cpp
    ${CMAKE_SOURCE_DIR}/thirdparty/sqlite3/sqlite3.c
)

target_include_directories(test
    PRIVATE
    ${CMAKE_SOURCE_DIR}/thirdparty/sqlite3
)

include(GNUInstallDirs)
install(TARGETS test
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
