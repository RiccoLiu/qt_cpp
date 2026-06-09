cmake_minimum_required(VERSION 3.16)

project(database2 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(unofficial-mysql-connector-cpp CONFIG REQUIRED)

add_library(database2 STATIC
    database2.cpp
    database2.h
    mysql_database.h mysql_database.cpp
    sqlite_database.h sqlite_database.cpp
    db_utils.h db_utils.cpp
    ${CMAKE_SOURCE_DIR}/thirdparty/sqlite3/sqlite3.c
)

target_link_libraries(database2
PRIVATE
    logger2
    unofficial::mysql-connector-cpp::connector
)

target_include_directories(database2
PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}             # 使用当前库的使用者可以直接包含 #include "database2.h"
PRIVATE
    ${CMAKE_SOURCE_DIR}/thirdparty/sqlite3  # 当前库可以包含的头文件目录
)

target_compile_definitions(database2 PRIVATE DATABASE2_LIBRARY)
