cmake_minimum_required(VERSION 3.16)

project(utils LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Eigen3 CONFIG REQUIRED)
find_package(OpenCV REQUIRED)

add_library(utils STATIC
    utils_global.h
    utils.cpp
    utils.h
    compiler.h
    singleton.h
    image_container.h
    fixedpoint.h
    image.h image.cpp
    sharedata.cpp
    sharedata.h
    pixel_type.h
    trait_format.h
    image_pyramid.cpp
    image_pyramid.h
)

target_link_libraries(utils
PUBLIC
    Eigen3::Eigen
PRIVATE
    logger2
    ${OpenCV_LIBS}
)

target_include_directories(utils
PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_definitions(utils PRIVATE UTILS_LIBRARY)
