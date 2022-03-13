#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include "Renderer/Camera/OrbitalCamera.h"
#include "Objects/CubeObject.h"

class GLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    GLWidget(QWidget *pWidget);
    virtual ~GLWidget() Q_DECL_OVERRIDE;

protected:
    const float fov = 60.0f;

    std::unique_ptr<QOpenGLShaderProgram> shader = nullptr;
    std::unique_ptr<QOpenGLBuffer> vb = nullptr;
    std::unique_ptr<QOpenGLBuffer> ib = nullptr;
    std::unique_ptr<QOpenGLVertexArrayObject> va = nullptr;

    std::unique_ptr<OrbitalCamera> Camera;
    std::unique_ptr<CubeObject> cube;
    QMatrix4x4 projectionMatrix;

    void paintGL() Q_DECL_OVERRIDE;
    void initializeGL() Q_DECL_OVERRIDE;
    void resizeGL(int w, int h) Q_DECL_OVERRIDE;

    void UpdateProjectionMatrix(float aspectRatio);
};

#endif // GLWIDGET_H
