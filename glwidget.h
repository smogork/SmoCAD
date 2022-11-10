#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>

#include <QMouseEvent>
#include <QWheelEvent>
#include <QKeyEvent>
#include <QOpenGLFunctions_4_4_Core>
#include <QOpenGLFramebufferObject>
#include <QOpenGLDebugLogger>

#include "Renderer/InputController/InputController.h"
#include "Renderer/InputController/InputEvents/CameraUpdateEvent.h"
#include "Renderer/Renderer.h"

class GLWidget : public QOpenGLWidget, public QOpenGLFunctions_4_4_Core
{
Q_OBJECT

private:
    bool m_isInitialized = false;
    std::unique_ptr<QOpenGLFramebufferObject> m_fbo = nullptr;
    std::unique_ptr<QOpenGLDebugLogger> m_logger;

private slots:
    void OnOpenGLError(const QOpenGLDebugMessage &debugMessage);

public:
    GLWidget(QWidget *pWidget);

    virtual ~GLWidget() Q_DECL_OVERRIDE;

    //  Przyklad wykorzystania customowego framebuffera aby wyrenderowac w QT do do tekstury
    // https://stackoverflow.com/a/31382768
    void DrawOffscreen(QSize bufferSize, std::function<void(QOpenGLContext* context)> renderFunction = {});

signals:
    void WidgetResized(QSize size);

public slots:

    void UpdateCameraSlot(std::shared_ptr<CameraUpdateEvent> event);
    void RedrawScreen();
    void showObjectListContextMenu(const QPoint &pos);

#pragma region QOpenGLWidget overrides
    void paintGL() Q_DECL_OVERRIDE;
    void initializeGL() Q_DECL_OVERRIDE;
    void resizeGL(int w, int h) Q_DECL_OVERRIDE;
#pragma endregion

    void LoadShaders();

#pragma region Mouse Handlers

    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE
    { Renderer::controller.mousePressSlot(event); }

    void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE
    { Renderer::controller.mouseReleaseSlot(event); }

    void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE
    { Renderer::controller.mouseMoveSlot(event); }

    void wheelEvent(QWheelEvent *event) Q_DECL_OVERRIDE
    { Renderer::controller.wheelSlot(event); }

#pragma endregion
};

#endif // GLWIDGET_H
