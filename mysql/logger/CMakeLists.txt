cmake_minimum_required(VERSION 3.16)

project(logger LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(logger SHARED
  logger_global.h
  logger.cpp
  logger.h
  ${CMAKE_SOURCE_DIR}/thirdparty/easylogging/easylogging++.cpp
  ${CMAKE_SOURCE_DIR}/thirdparty/easylogging/easylogging++.h
)

target_include_directories(logger
PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
PRIVATE
    ${CMAKE_SOURCE_DIR}/thirdparty/easylogging
)
target_compile_definitions(logger PRIVATE LOGGER_LIBRARY)
