cmake_minimum_required(VERSION 3.16)

project(logger2 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# find_package(spdlog REQUIRED)
find_package(spdlog CONFIG REQUIRED)

add_library(logger2 SHARED
    logger2_global.h
    logger2.cpp
    logger2.h
)

target_link_libraries(logger2
PRIVATE
    spdlog::spdlog
)

target_include_directories(logger2
PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_definitions(logger2 PRIVATE LOGGER2_LIBRARY)

add_subdirectory(test)
