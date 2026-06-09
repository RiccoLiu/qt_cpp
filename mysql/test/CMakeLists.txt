cmake_minimum_required(VERSION 3.16)

project(test LANGUAGES C CXX)

# set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(unofficial-mysql-connector-cpp CONFIG REQUIRED)
find_package(OpenCV REQUIRED)
find_package(Eigen3 CONFIG REQUIRED)
find_package(yaml-cpp REQUIRED)

add_executable(test
    main.cpp
    ${CMAKE_SOURCE_DIR}/thirdparty/sqlite3/sqlite3.c
)

target_include_directories(test
PRIVATE
    ${CMAKE_SOURCE_DIR}/thirdparty/sqlite3
)

target_link_libraries(test
PRIVATE
    logger2              # logger
    unofficial::mysql-connector-cpp::connector  # mysql-connector
    ${OpenCV_LIBS}
    Eigen3::Eigen
    yaml-cpp
)

include(GNUInstallDirs)
install(TARGETS test
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
