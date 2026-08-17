#include "mygl.h"

// myglwidget.cpp
MyGLWidget::MyGLWidget(QWidget *parent) : QOpenGLWidget(parent) {}

MyGLWidget::~MyGLWidget() {
    makeCurrent(); // ⚠️ 析构时必须确保上下文激活
    glDeleteVertexArrays(1, &m_vao);
    glDeleteBuffers(1, &m_vbo);
    doneCurrent();
}

void MyGLWidget::initializeGL() {
    initializeOpenGLFunctions(); // 初始化函数指针

    // 设置背景色
    glClearColor(0.1f, 0.1f, 0.15f, 1.0f);

    // 编译着色器程序
    m_program = std::make_unique<QOpenGLShaderProgram>(this);
    m_program->addShaderFromSourceCode(QOpenGLShader::Vertex, R"(
        #version 330 core
        layout(location=0) in vec3 aPos;
        void main() { gl_Position = vec4(aPos, 1.0); }
    )");
    m_program->addShaderFromSourceCode(QOpenGLShader::Fragment, R"(
        #version 330 core
        out vec4 fragColor;
        void main() { fragColor = vec4(0.2, 0.7, 1.0, 1.0); }
    )");
    m_program->link();

    // 三角形顶点数据
    float vertices[] = { -0.5f,-0.5f,0,  0.5f,-0.5f,0,  0,0.5f,0 };

    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), nullptr);
    glEnableVertexAttribArray(0);
}

void MyGLWidget::resizeGL(int w, int h) {
    glViewport(0, 0, w, h);
}

void MyGLWidget::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT);
    m_program->bind();
    glBindVertexArray(m_vao);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    m_program->release();
}
