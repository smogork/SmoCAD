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

#include "Renderer/Camera/OrbitalCamera.h"
#include "Objects/CubeObject.h"

#define UNDEFINED_ID -1
#define LMOUSE_ID 0
#define MMOUSE_ID 1
#define RMOUSE_ID 2

#define MOVE_SENSITIVITY 0.005f
#define ROTATE_SENSITIVITY 0.005f
#define ZOOM_SENSITIVITY 0.001f

class GLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    GLWidget(QWidget *pWidget);
    virtual ~GLWidget() Q_DECL_OVERRIDE;

protected:
    const float fov = 60.0f;
    QPoint lastMousePos;
    bool mousePresses[3];

    std::unique_ptr<QOpenGLShaderProgram> shader = nullptr;
    std::unique_ptr<QOpenGLBuffer> vb = nullptr;
    std::unique_ptr<QOpenGLBuffer> ib = nullptr;
    std::unique_ptr<QOpenGLVertexArrayObject> va = nullptr;

    std::unique_ptr<OrbitalCamera> camera;
    std::unique_ptr<CubeObject> cube;
    QMatrix4x4 projectionMatrix;

    void paintGL() Q_DECL_OVERRIDE;
    void initializeGL() Q_DECL_OVERRIDE;
    void resizeGL(int w, int h) Q_DECL_OVERRIDE;

    //[TODO] dodać jakąś oddzielną klase do przechwytywanie IO i zronbienie bardziej wyspecjalizowanych sygnalow
    void mousePressEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
    void wheelEvent(QWheelEvent* event) Q_DECL_OVERRIDE;
    void keyPressEvent(QKeyEvent* event) Q_DECL_OVERRIDE;
    void keyReleaseEvent(QKeyEvent* event) Q_DECL_OVERRIDE;
    int translateMouseButton(Qt::MouseButton button);

    void UpdateProjectionMatrix(float aspectRatio);
};

#endif // GLWIDGET_H
