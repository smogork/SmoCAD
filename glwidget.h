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
#include "Renderer/InputController/InputEvents/CameraUpdateEvent.h"

#include <Renderer/ShaderWrapper.h>

#include "Renderer/Camera/OrbitalCamera.h"
#include "Objects/CubeObject.h"
#include "Objects/TorusObject.h"
#include "Renderer/Camera/Viewport.h"
#include "Objects/CursorObject.h"


class GLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    GLWidget(QWidget *pWidget);
    virtual ~GLWidget() Q_DECL_OVERRIDE;

    void UpdateTorusObjectTransform(QVector3D pos, QVector3D rot, QVector3D scale);
    void UpdateTorusObjectParameters(float R, float r, int Rdensity, int rdensity);
    void AddSelectableObject(IRenderableObject* ro);
    void DeleteSelectableObject(IRenderableObject* ro);

    std::shared_ptr<ShaderWrapper> shader = nullptr;

public slots:
    void UpdateCameraSlot(std::shared_ptr<CameraUpdateEvent> event);
    void MouseRaycastSlot(std::shared_ptr<SceneMouseClickEvent> event);

protected:
    std::unique_ptr<InputController> controls = nullptr;
    std::shared_ptr<Viewport> viewport = nullptr;


    std::shared_ptr<ShaderWrapper> shader2 = nullptr;

    std::list<IRenderableObject*> renderableObjects;
    std::unique_ptr<CursorObject> cursor = nullptr;

    void paintGL() Q_DECL_OVERRIDE;
    void initializeGL() Q_DECL_OVERRIDE;
    void resizeGL(int w, int h) Q_DECL_OVERRIDE;

    void DrawRenderableObject(IRenderableObject *ro, std::shared_ptr<ShaderWrapper> shader);

    void mousePressEvent(QMouseEvent* event) Q_DECL_OVERRIDE {this->controls->mousePressSlot(event);}
    void mouseReleaseEvent(QMouseEvent* event) Q_DECL_OVERRIDE {this->controls->mouseReleaseSlot(event);}
    void mouseMoveEvent(QMouseEvent* event) Q_DECL_OVERRIDE {this->controls->mouseMoveSlot(event);}
    void wheelEvent(QWheelEvent* event) Q_DECL_OVERRIDE {this->controls->wheelSlot(event);}
    void keyPressEvent(QKeyEvent* event) Q_DECL_OVERRIDE {this->controls->keyPressSlot(event);}
    void keyReleaseEvent(QKeyEvent* event) Q_DECL_OVERRIDE {this->controls->keyReleaseSlot(event);}
};

#endif // GLWIDGET_H
