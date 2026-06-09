cmake_minimum_required(VERSION 3.16)

project(pipeline2 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(yaml-cpp REQUIRED)

add_library(pipeline2 STATIC
    pipeline.cpp pipeline.h
    pipeline_msg.cpp pipeline_msg.h
    message_broker.cpp message_broker.h
    pipeline_node.cpp pipeline_node.h
    pipeline_node_factory.cpp pipeline_node_factory.h
)
target_link_libraries(pipeline2
PRIVATE
    logger2              # logger
    yaml-cpp
)
target_include_directories(pipeline2
PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}             # 使用当前库的使用者可以直接包含 #include "database2.h"
)

# target_compile_definitions(pipeline2 PRIVATE PIPELINE_LIBRARY)
add_subdirectory(test)
