#ifndef MYGL_H
#define MYGL_H

#include <QOpenGLContext>
#include <QtOpenGLWidgets/QOpenGLWidget>
#include <QtOpenGL/QOpenGLShaderProgram>
#include <QtOpenGL/QOpenGLFunctions_3_3_Core>

// myglwidget.h
class MyGLWidget : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core {
    Q_OBJECT
public:
    explicit MyGLWidget(QWidget *parent = nullptr);
    ~MyGLWidget() override;

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

private:
    GLuint m_vao = 0, m_vbo = 0;
    std::unique_ptr<QOpenGLShaderProgram> m_program;
};


#endif // MYGL_H
