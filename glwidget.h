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
#include "Objects/IRenderableObject.h"
#include "Objects/BezierCurveC0.h"
#include "Renderer/Renderer.h"

class GLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
Q_OBJECT
public:
    GLWidget(QWidget *pWidget);

    virtual ~GLWidget() Q_DECL_OVERRIDE;

signals:
    void WidgetResized(QSize size);

public slots:

    void UpdateCameraSlot(std::shared_ptr<CameraUpdateEvent> event);
    void RedrawScreen();

#pragma region QOpenGLWidget overrides
    void paintGL() Q_DECL_OVERRIDE;

    void initializeGL() Q_DECL_OVERRIDE;

    void resizeGL(int w, int h) Q_DECL_OVERRIDE;
#pragma endregion

    void LoadShaders();
    //void UpdateUniforms();

    void DrawRenderableObject(IRenderableObject *ro, std::shared_ptr<ShaderWrapper> shader,
                              const std::function<void(ShaderWrapper *)> &uniformOverrides = {});

    void DrawBezier(BezierCurveC0 *bezier, const std::function<void(ShaderWrapper *)> &uniformOverrides = {});

#pragma region Mouse Handlers
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE { Renderer::controller.mousePressSlot(event); }

    void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE { Renderer::controller.mouseReleaseSlot(event); }

    void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE { Renderer::controller.mouseMoveSlot(event); }

    void wheelEvent(QWheelEvent *event) Q_DECL_OVERRIDE { Renderer::controller.wheelSlot(event); }
#pragma endregion
};

#endif // GLWIDGET_H
