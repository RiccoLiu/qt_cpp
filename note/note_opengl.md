# 搭建开发环境

GLFW 下载地址: https://www.glfw.org/download.html  
GLAD 下载地址: https://glad.dav1d.de/
- Language: c/c++
- Specification: OpenGL
- API: 
- - gl: Version 3.3
- Profile: Core
- Options
- - Generate a loader

点击 GENERATE 生成 GLAD 头文件和源码

将GLFW 和 GLAD 头文件、库文件、源码添加到工程后，编译，链接后，运行测试成功。

# OpengGL 
## VAO & VBO & EBO

| 对象 | 一句话理解 | 类比 |
| :--- | :--- | :--- |
| VBO | GPU 上的数据仓库 | 零件箱 |
| EBO | 顶点的复用索引表 | 组装说明书 |
| VAO | 所有顶点状态的快照 | 贴好标签的完整包裹 |

## GLSL 着色器 Shader

layout: 指定为顶点着色器  
localtion: 用于顶点着色器，指定输入变量    
in: 指定此着色器的输入变量   
out: 指定着色器的输出变量  
uniform：指定着色器程序的全局变量，此变量常用于CPU 和着色器之间的数据传递。 

着色器典型程序如下：

```
#version version_number

layout (location = 0) in vec3 aPos
in type in_variable_name;
in type in_variable_name;

out type out_variable_name;

uniform type uniform_name;

void main()
{
  // 处理输入并进行一些图形操作
  ...
  // 输出处理过的结果到输出变量
  gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0f);
}
```

CPU程序获取 uniform 变量和赋值：

```
float timeValue = glfwGetTime();
float greenValue = (sin(timeValue) / 2.0f) + 0.5f; // 调整到 0-1 之间
int vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");
glUseProgram(shaderProgram);
glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);
```

## 纹理 Texture

纹理的坐标范围: [0, 0] -> [1, 1]
 


| 纹理的环绕方式 | 描述 |
|----------|-----|
| GL_REPEAT | 对纹理的默认行为。重复纹理图像。 |
| GL_MIRRORED_REPEAT | 和GL_REPEAT一样，但每次重复图片是镜像放置的。 |
| GL_CLAMP_TO_EDGE | 纹理坐标会被约束在0到1之间，超出的部分会重复纹理坐标的边缘，产生一种边缘被拉伸的效果。 |
| GL_CLAMP_TO_BORDER | 超出的坐标为用户指定的边缘颜色。 |
	
	
	
	
