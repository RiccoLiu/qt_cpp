cmake_minimum_required(VERSION 3.16)

project(database LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(database STATIC # 使用静态库替换动态库 SHARED
  database_global.h
  database.cpp
  database.h
  ${CMAKE_SOURCE_DIR}/thirdparty/sqlite3/sqlite3.c
  table.h
  table.cpp
  usertable.h usertable.cpp
  filmtable.h filmtable.cpp
  tablefactory.h
  sqlprocessor.h
  sqliteprocessor.h sqliteprocessor.cpp
  sqlfactory.h
)

target_link_libraries(database PRIVATE
  logger
)

target_include_directories(database
  PUBLIC
      ${CMAKE_CURRENT_SOURCE_DIR}  # 让使用者能 include "database.h"
  PRIVATE
      ${CMAKE_SOURCE_DIR}/thirdparty/sqlite3  # 让 database.cpp 能 #include "sqlite3.h"
)

# target_compile_definitions(database PRIVATE DATABASE_LIBRARY)
