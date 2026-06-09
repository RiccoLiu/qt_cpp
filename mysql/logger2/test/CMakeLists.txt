cmake_minimum_required(VERSION 3.16)

project(logger2_test LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(logger2_test
    main.cpp
)

target_link_libraries(logger2_test
    logger2
    spdlog::spdlog

)

