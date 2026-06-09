cmake_minimum_required(VERSION 3.16)

project(utils LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(utils STATIC
    utils_global.h
    utils.cpp utils.h
    compiler.h
    sharedt.h
    singleton.h
    Thread.h Thread.cpp
)

target_include_directories(utils
PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_definitions(utils PRIVATE UTILS_LIBRARY)
