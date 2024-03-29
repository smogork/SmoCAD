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
#include <QOpenGLTexture>

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
    // Zwraca handle na teksture z wygenerowanym Zbufforem
    std::shared_ptr<QOpenGLTexture>
    DrawOffscreen(QSize bufferSize, std::function<void(QOpenGLContext *context)> renderFunction = {});
    std::shared_ptr<QOpenGLTexture> CreateDepthTexture(QSize size,
                                                       QOpenGLTexture::TextureFormat format = QOpenGLTexture::D24S8);
    std::shared_ptr<QOpenGLTexture> CreateFloatTexture32(QSize size);
    template<class T>
    std::shared_ptr<QOpenGLBuffer> CreateComputeBuffer(int count)
    {
        std::shared_ptr<QOpenGLBuffer> res = std::make_shared<QOpenGLBuffer>();
        res->create();
        res->setUsagePattern(QOpenGLBuffer::DynamicCopy);
        res->bind();
        res->allocate(sizeof(T) * count);
        res->release();
        return res;
    }

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
