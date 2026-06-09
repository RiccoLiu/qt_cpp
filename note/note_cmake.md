

## 基础 CMakeLists.txt

- 动态库

```
cmake_minimum_required(VERSION 3.16)

project(database LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(database SHARED
  database_global.h
  database.cpp
  database.h
  ${CMAKE_SOURCE_DIR}/thirdparty/sqlite3/sqlite3.c
)

target_include_directories(database
  PUBLIC
      ${CMAKE_CURRENT_SOURCE_DIR}  # 让使用者能 include "database.h"
  PRIVATE
      ${CMAKE_SOURCE_DIR}/thirdparty/sqlite3  # 让 database.cpp 能 #include "sqlite3.h"
)

target_compile_definitions(database PRIVATE DATABASE_LIBRARY) # 定义 DATABASE_LIBRARY 宏，实现动态库跨平台支持

```

- 应用程序 

```
cmake_minimum_required(VERSION 3.19)
project(mysql LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets)

qt_standard_project_setup()

<!-- include_directories(
    ${CMAKE_SOURCE_DIR}/database    # 由于 动态库中使用了 target_include_directories 链接库时可以自动获取头文件路径
) -->

qt_add_executable(mysql
    WIN32 MACOSX_BUNDLE
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
)

target_link_libraries(mysql
    PRIVATE
        Qt::Core
        Qt::Widgets
        database        # 链接后自动获得 PUBLIC include 路径
)

include(GNUInstallDirs)

install(TARGETS mysql
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET mysql
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
add_subdirectory(database)
```

## 预定义变量

| 工程构建 | 含义 | 示例 |
|---------|------|------|
| CMAKE_SOURCE_DIR | 工程根目录 | /home/project |
| CMAKE_CURRENT_SOURCE_DIR | 当前 CMakeLists.txt 目录 |  /home/project/database |
| CMAKE_BINARY_DIR | 构建程序的顶层目录 | /home/project/build |
| CMAKE_CURRENT_BINARY_DIR | 构建程序的子目录 | /home/project/build/database |
| PROJECT_SOURCE_DIR | 当期project()命令所在目录 |  /home/project/database |
| PROJECT_BINARY_DIR | 当前project()命令对应的构建目录 | /home/project/build/database |

| 系统和平台 | 含义 | 示例 |
|---------- |------|-----|
| CMAKE_SYSTEM_NAME | 操作系统 | Linux, Windows, Darwin (macOS) |
| CMAKE_HOST_SYSTEM_NAME | 构建操作系统 | 同上 |
| CMAKE_SIZEOF_VOID_P | 指针大小 | 8（64位）, 4（32位） |
| CMAKE_BUILD_TYPE | 构建类型 | Debug, Release, RelWithDebInfo |
| CMAKE_GENERATOR | 当前使用的生成器 | Unix Makefiles, Ninja, Visual Studio 17 2022 |

| 编译器和语言 | 含义 | 示例 |
|-------------|------|-----|
| CMAKE_CXX_COMPILER_ID | C++ 编译器标识 | MSVC, Clang, gcc |
| CMAKE_C_COMPILER_ID | C 编译器标识 |  |
| CMAKE_CXX_STANDARD | C++ 标准 | |
| CMAKE_CXX_FLAGS | 全局 C++ 编译选项(不推荐直接修改) |  |

| 安装与打包 | 含义 | 示例 |
|-------------|------|-----|
| CMAKE_INSTALL_PREFIX | 安装根目录 | /usr/local (Linux), C:/Program Files (Windows) |
| CMAKE_INSTALL_BINDIR | 可执行文件安装目录 | bin |
| CMAKE_INSTALL_LIBDIR | 库文件安装目录 | lib |
| CMAKE_INSTALL_INCLUDEDIR | 头文件安装目录 | include |

| 其他使用变量 | 含义 | 示例 |
|-------------|------|-----|
| ${CMAKE_VERSION} | CMake 版本 | 3.25.0 |
| PROJECT_NAME | 当前 project(database) 中的名字 | database |
| PROJECT_VERSION | 项目版本 | 项目需要设置VERSION |
| CMAKE_PROJECT_NAME | 最外层项目名称  | mysql|

## 命令

| 命令 | 作用 | 使用场景 | 示例 |
|------|------|---------|------|
| add_definitions | 所有 target 添加预定义宏 | 预定义宏容易污染 | add_definitions(-DFOO -DBAR=1) |
| target_compile_definitions |  指定 target 添加预定义宏 | PRIVATE: 仅target内部可以看到, PUBLIC: target 内部 和 外部链接者都能看到，INTERFACE: 仅链接者能看到 | target_compile_definitions(target PRIVATE DATABASE_LIBRARY) |
| include_directories | 所有target添加include路径 | 必须在 add_executable 前使用 | include_directories(path1 path2) |
| target_include_directories | 指定target 添加include 路径 | PRIVATE: 仅target内部可以看到, PUBLIC: target 内部 和 外部链接者都能看到，INTERFACE: 仅链接者能看到 | target_include_directories(target path1 path2) |


## 调试

1. 如何获取 include 属性

```
# 获取 include_directories 的属性
get_target_property(mysql_includes mysql INCLUDE_DIRECTORIES)
message(STATUS "mysql includes: ${mysql_includes}")

# 获取 target_include_directories 属性
get_target_property(mysql_includes mysql INTERFACE_INCLUDE_DIRECTORIES)
message(STATUS "mysql INTERFACE includes: ${mysql_includes}")

# 查看 database 库导出的 PUBLIC 路径
get_target_property(db_public_includes database INTERFACE_INCLUDE_DIRECTORIES)
message(STATUS "database PUBLIC includes: ${db_public_includes}")

```

