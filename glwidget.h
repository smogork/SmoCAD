#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>

#include <QMouseEvent>
#include <QWheelEvent>
#include <QKeyEvent>

#include <Renderer/InputController/InputController.h>
#include <Renderer/InputController/cameramovementevent.h>

#include <Renderer/ShaderWrapper.h>

#include "Renderer/Camera/OrbitalCamera.h"
#include "Objects/CubeObject.h"
#include "Objects/TorusObject.h"



class GLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    GLWidget(QWidget *pWidget);
    virtual ~GLWidget() Q_DECL_OVERRIDE;

    std::unique_ptr<TorusObject> torus;
    void UpdateTorusObjectTransform(QVector3D pos, QVector3D rot, QVector3D scale);
    void UpdateTorusObjectParameters(float R, float r, int Rdensity, int rdensity);

public slots:
    void UpdateCameraSlot(std::shared_ptr<CameraMovementEvent> event);

protected:
    std::unique_ptr<InputController> controls = nullptr;

    const float fov = 60.0f;
    QMatrix4x4 projectionMatrix;

    std::unique_ptr<ShaderWrapper> shader = nullptr;
    //std::unique_ptr<QOpenGLShaderProgram> shader = nullptr;
    std::unique_ptr<QOpenGLBuffer> vb = nullptr;
    std::unique_ptr<QOpenGLBuffer> ib = nullptr;
    std::unique_ptr<QOpenGLVertexArrayObject> va = nullptr;
    //std::unique_ptr<CubeObject> cube;


    void paintGL() Q_DECL_OVERRIDE;
    void initializeGL() Q_DECL_OVERRIDE;
    void resizeGL(int w, int h) Q_DECL_OVERRIDE;

    void mousePressEvent(QMouseEvent* event) Q_DECL_OVERRIDE {this->controls->mousePressSlot(event);}
    void mouseReleaseEvent(QMouseEvent* event) Q_DECL_OVERRIDE {this->controls->mouseReleaseSlot(event);}
    void mouseMoveEvent(QMouseEvent* event) Q_DECL_OVERRIDE {this->controls->mouseMoveSlot(event);}
    void wheelEvent(QWheelEvent* event) Q_DECL_OVERRIDE {this->controls->wheelSlot(event);}
    void keyPressEvent(QKeyEvent* event) Q_DECL_OVERRIDE {this->controls->keyPressSlot(event);}
    void keyReleaseEvent(QKeyEvent* event) Q_DECL_OVERRIDE {this->controls->keyReleaseSlot(event);}


    void UpdateProjectionMatrix(float aspectRatio);
};

#endif // GLWIDGET_H
