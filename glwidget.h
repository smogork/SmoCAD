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
#include "Scene/SceneModelOld.h"

#define DEFAULT_SHADER 0
#define CURSOR_SHADER 1
#define BEZIER_SHADER 2

class GLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
Q_OBJECT
public:
    GLWidget(QWidget *pWidget);

    virtual ~GLWidget() Q_DECL_OVERRIDE;

    void SetupSceneAndControls(std::shared_ptr<InputController> controler, std::shared_ptr<SceneModelOld> model);

signals:

    void WidgetResized(QSize size);

public slots:

    void UpdateCameraSlot(std::shared_ptr<CameraUpdateEvent> event);

protected:

    std::shared_ptr<SceneModelOld> scene = nullptr;
    std::shared_ptr<InputController> controls = nullptr;

    std::vector<std::shared_ptr<ShaderWrapper>> shaders;

    void paintGL() Q_DECL_OVERRIDE;

    void initializeGL() Q_DECL_OVERRIDE;

    void resizeGL(int w, int h) Q_DECL_OVERRIDE;

    void DrawRenderableObject(IRenderableObject *ro, std::shared_ptr<ShaderWrapper> shader,
                              const std::function<void(ShaderWrapper *)> &uniformOverrides = {});

    void DrawBezier(BezierCurveC0 *bezier, const std::function<void(ShaderWrapper *)> &uniformOverrides = {});

    void InitializeUniforms();

    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE { this->controls->mousePressSlot(event); }

    void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE { this->controls->mouseReleaseSlot(event); }

    void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE { this->controls->mouseMoveSlot(event); }

    void wheelEvent(QWheelEvent *event) Q_DECL_OVERRIDE { this->controls->wheelSlot(event); }

};

#endif // GLWIDGET_H
