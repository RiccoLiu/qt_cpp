cmake_minimum_required(VERSION 3.16)

project(pipeline LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(yaml-cpp REQUIRED)

add_library(pipeline STATIC
    pipeline_msg.cpp
    pipeline_msg.h
    pipeline.cpp
    pipeline.h
    source.h source.cpp
    sink.h sink.cpp
    pipeline_thread.h pipeline_thread.cpp
    pipeline_node.h pipeline_node.cpp
    pipeline_sys_msg.h
)
target_link_libraries(pipeline
PRIVATE
    logger2              # logger
    yaml-cpp
PUBLIC
    utils
)
target_include_directories(pipeline
PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}             # 使用当前库的使用者可以直接包含 #include "database2.h"
)

target_compile_definitions(pipeline PRIVATE PIPELINE_LIBRARY)

add_subdirectory(test)

