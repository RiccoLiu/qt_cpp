cmake_minimum_required(VERSION 3.16)

project(pipeline_test LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenCV REQUIRED)
find_package(yaml-cpp REQUIRED)

add_executable(pipeline_test
    main.cpp
    camera_msg.h camera_msg.cpp
    video_decoder.cpp video_decoder.h
    node_factory.h node_factory.cpp
    opencv_display.h opencv_display.cpp
    face_detector.h face_detector.cpp
)

target_link_libraries(pipeline_test
    ${OpenCV_LIBS}
    logger2
    pipeline
    yaml-cpp
    utils
)

