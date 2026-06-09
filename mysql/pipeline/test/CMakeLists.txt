cmake_minimum_required(VERSION 3.16)

project(pipeline_test LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenCV REQUIRED)
find_package(yaml-cpp REQUIRED)

add_executable(pipeline_test
    main.cpp

    opencv_frame.cpp opencv_frame.h
    video_decoder.cpp video_decoder.h
    opencv_display.cpp opencv_display.h
    face_detector.cpp face_detector.h
)

target_link_libraries(pipeline_test
    ${OpenCV_LIBS}
    logger2
    pipeline
    yaml-cpp
)

