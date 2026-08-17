
# Mobaxterm 配置

## SSH Key

```
ssh-keygen -t rsa -C "liuchong12233@163.com"
ssh key: /home/Administrator/.ssh/id_rsa.pub
```

## Git 配置

```
git config --global user.email "liuchong12233@163.com"
git config --global user.name "liuchong"

git config --global core.editor vim
git config --global alias.st status
git config --global alias.co checkout
git config --global alias.ci commit
git config --global alias.br branch
git config --global color.ui auto
```

## SVN

查看版本信息
    svn --version

查看文件状态	
    svn status | findstr "^M"

# VCPKG

## 安装 VCPKG

```
// 克隆仓库（建议放在固定位置，如 C:\vcpkg）
git clone https://github.com/microsoft/vcpkg.git
cd vcpkg

// 运行引导脚本（会自动下载编译所需工具）
.\bootstrap-vcpkg.bat
```

## VCPKG 常用命令

```
// 列出已安装的所有软件
vcpkg list

// 查看可用的安装包
vcpkg search mysql

// 安装软件
vcpkg install mysql-connector-cpp:x64-windows           // 64 位动态链接
vcpkg install mysql-connector-cpp:x86-windows           // 32 位动态链接
vcpkg install mysql-connector-cpp:x64-windows-static    // 64 位静态链接

vcpkg.exe install "opencv[contrib,nonfree]:x64-windows"   // 安装Opencv

// 卸载软件
vcpkg remove eigen3:x64-windows

// 集成到 Visual Studio
vcpkg integrate install

// 集成到 CMake
// 1. 指定 CMAKE_TOOLCHAIN_FILE
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake

// 2. CMakeList.txt 使用安装包
find_package(mysql-connector-cpp CONFIG REQUIRED)
target_link_libraries(myapp PRIVATE mysqlcppconn)

```

## VCPKG Manifest 

编写 vcpkg.json 描述项目依赖的第三方库

```
{
  "name": "mysql",
  "version": "1.0.0",
  "description": "Project using official MySQL Connector/C++ and OpenCV with contrib/nonfree",
  "dependencies": [
    {
      "name": "opencv",
      "features": ["contrib", "nonfree"]
    },
    "mysql-connector-cpp"
  ]
}
```

执行cmake 生成 vcpkg.lock
```
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=path/to/vcpkg/scripts/buildsystems/vcpkg.cmake
```

# 杂项
## windows 环境下制作动态库

Common.h 导出的示例代码如下所示：

```
//  编译器预定义：DLL_EXPORTS
#include <cstdint>

#ifdef _WIN32
#ifdef DLL_EXPORTS
#define _API_ __declspec(dllexport)
#else
#define _API_ __declspec(dllimport)
#endif
#else
#define _API_ 
#endif

#ifdef __cplusplus  // 如果此工程使用c++编译器，这个函数会按照C函数去导出
extern "C" {
#endif

// 跨模块传递的对象使用 POD (Plain Old Data: 清楚旧数据) 或者 句柄 (void*)
// 常见的POD类型: int, float, double, bool, char*, struct等

_API_ void WriteImgToFile(const char* filename, std::uint16_t* img_buffer, int img_width, int img_height);

#ifdef __cplusplus
}
#endif
```

总结:  
1. window CRT 统一使用 /MD 或者 /MT (推荐使用/MD)
2. MSVC 不承诺 STL ABI 稳定，跨模块传递的对象不能使用STL对象，只能是POD或者句柄(void*), 使用STL的场景需要使用PIMPL结构封装起来


```
// 1. C 风格的句柄 + POD类型的

// .h 定义对外 C 类型 句柄 + POD 的接口
typedef void* SEGMENT_HANDLE;

#ifdef __cplusplus  // 如果此工程使用c++编译器，这个函数会按照C函数去导出
extern "C" {
#endif

typedef void* SEGMENT_HANDLE; // 也可以使用结构体 typedef struct SegmentHandleTag* SEGMENT_HANDLE;

// C 风格 API
SEGMENT_HANDLE create_handle();
void do_something(SEGMENT_HANDLE h);
void destroy_handle(SEGMENT_HANDLE h);


#ifdef __cplusplus
}
#endif

// .cpp 内部实现, class SegmentRegionImpl 转换成void*, 如果是 struct SegmentHandleTag* SEGMENT_HANDLE; cpp中要实现 SegmentHandleTag 这个结构
class SegmentRegionImpl {
public:
    SegmentRegionImpl() {}
    virtual ~SegmentRegionImpl() {}
    void do_something() {}
};

SEGMENT_HANDLE create_handle(...) {
    return new SegmentRegionImpl();
}

void detroy_handle(SEGMENT_HANDLE h) {
    delete static_cast<SegmentRegionImpl*>(h);
}

void do_something(SEGMENT_HANDLE h) {
    SegmentRegionImpl* pimpl = static_cast<SegmentRegionImpl*>(h);
    pimpl->do_something();
}

// 2. PIMPL 结构封装导外部直接构造 SegmentRegion，头文件会有 Impl 的信息
class _API_ SegmentRegion {
public:
    SegmentRegion();
    ~SegmentRegion();

    int do_something() const;

private:
    struct Impl;  // 所有的STL结构都封装到Impl结构里，对外的头文件中不暴露
    Impl* impl_;  
};

// 3. 对外SDK 更建议使用 接口 + 工厂 这种可以保证ABI更稳定
// .h 设定接口 
class SegmentRegion {
public:
    ~SegmentRegion();

    virtual int do_something() = 0;
    virtual rvoid release() = 0;
};

_API_ SegmentRegion* CreateSegmentRegion();

// .cpp 实现
class SegmentRegionImpl : public SegmentRegion {
public:
    SegmentRegionImpl() {}
    ~SegmentRegionImpl() {}

    virtual int do_something() overide {
    }
    
    virtual void release() overide {
        delete this;
    }
}

SegmentRegion* CreateSegmentRegion() {
    return new SegmentRegionImpl();
}

// 外部使用
SegmentRegion* sr = CreateSegmentRegion();
sr->do_something();
sr->release();


```
3. 静态库是直接打包进exe的，如果MSVC版本一致、CRT一致、STL一致、Debug\Release版本一致，静态库是可以暴露STL的接口
4. 静态库链接到多个DLL或EXE时，会导致一个进程出现多个实例，如果希望全进程唯一实例考虑使用 动态库 +  __declspec(dllexport) 的方式

# 报错


## visual studio 编译报错
```
static_assert failed: 'Unicode support requires compiling with /utf-8'
Build errors occurred.
```

修复方式:
```
在 Solution Explorer（解决方案资源管理器） 中，右键点击你的项目 → Properties（属性）；
左侧选择：Configuration Properties → C/C++ → Command Line；
在右侧的 Additional Options（附加选项） 中，手动添加：

/utf-8

重新编译
```



# Q&A

Q1. 为什么 Linux 下 STL 跨库没有问题？

| 系统环境 | Linux | Windows |
|---------|-------| -------|
| 编译器 | gcc | msvc |
| 标准库 | libstd++(ABI兼容) | MSVC STL(ABI不兼容) |
| 运行库 | glibc | 多种 CRT |
| 系统层 | Linue Kernel | Windows Kernel |

1. 运行时全进程只有一个 glibc, malloc/free/pthread 等底层函数是全局唯一的
2. 各个动态库和应用程序之间共享同一个 libstdc++.so 使STL 分配器是相同的
3. ABI 相对window 更稳定

Q2. 编译动态库导出文件说明

| 类别 | 文件 | 说明 |
|---|---|----|
| 静态库 | .lib | 包含所有编译后的目标代码（.obj 的集合），链接时直接嵌入到最终 EXE 中 | 
|  | .pdb | 存储调试符号（函数名、行号等），用于调试静态库代码（仅在开启调试信息时生成 |
| 动态库 | .lib | 仅包含函数符号和重定位信息，供链接器在编译其他项目时使用 |
|  | .dll | 包含实际可执行代码，运行时由操作系统加载 |
|  | .pdb | 存储 DLL 的调试符号，用于崩溃分析或调试 |
|  | .exp | 链接过程中的临时文件   |
| 编译后的机器码(二进制文件) | .obj |  |
| 可执行文件 | .exe | 将多个编译后的.obj文件链接成.exe文件 |

Q3. windows应用程序运行时依赖的dll文件，应该放在exe同一级目录，还是放在执行应用程序的当前工作目录？

应该放在exe的同一级目录


