#include <QOpenGLFunctions>
#include "glwidget.h"
#include "Objects/CursorObject.h"

GLWidget::GLWidget(QWidget *pWidget)
    : QOpenGLWidget(pWidget)
{
    QSurfaceFormat format;
    format.setRenderableType(QSurfaceFormat::OpenGL);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setVersion(3,3);
    setFormat(format);

    viewport = std::make_shared<Viewport>(size(), 60.0f);
    controls = std::make_unique<InputController>(viewport, this);


    QObject::connect(this->controls.get(), &InputController::CameraUpdated,
                     this, &GLWidget::UpdateCameraSlot);
    QObject::connect(this->controls.get(), &InputController::SceneMouseClicked,
                     this, &GLWidget::MouseRaycastSlot);
}

void GLWidget::initializeGL()
{
    initializeOpenGLFunctions();

    shader = std::make_shared<ShaderWrapper>("Shaders/uniform_color.vert", "Shaders/simple_color.frag");
    shader2 = std::make_shared<ShaderWrapper>("Shaders/buffer_color.vert", "Shaders/simple_color.frag");

    renderableObjects.push_back(new CubeObject(QVector3D(0.0f, 5.0f, 5.0f), shader));
    renderableObjects.push_back(new CubeObject(QVector3D(0.0f, 0.0f, 5.0f), shader));
    renderableObjects.push_back(new CursorObject(QVector3D(0.0f, 0.0f, 0.0f), shader2));
    renderableObjects.push_back(new TorusObject(QVector3D(5.0f, 0.0f, 10.0f), shader, 5, 1, 36, 18));
    renderableObjects.push_back(new CursorObject(QVector3D(0.0f, -5.0f, 0.0f), shader2));

    shader->SetUniform("u_MVP.View", controls->Camera->GetViewMatrix());
    shader->SetUniform("u_MVP.Projection", viewport->GetProjectionMatrix());
    shader->SetUniform("u_ObjectColor", QVector4D(1.0f, 0.5f, 0.2f, 1.0f));

    shader2->SetUniform("u_MVP.View", controls->Camera->GetViewMatrix());
    shader2->SetUniform("u_MVP.Projection", viewport->GetProjectionMatrix());
}

void GLWidget::resizeGL(int w, int h)
{
    QOpenGLWidget::resizeGL(w,h);

    viewport->UpdatePerspectiveMatrix(QSize(w, h));
    shader->SetUniform("u_MVP.Projection", viewport->GetProjectionMatrix());
    shader2->SetUniform("u_MVP.Projection", viewport->GetProjectionMatrix());
}

void GLWidget::paintGL()
{
    // set the background color = clear color
    glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (IRenderableObject* ro : renderableObjects)
    {
        ro->Bind();
        glDrawElements(GL_LINES, ro->GetIndexCount(), GL_UNSIGNED_INT, 0);
        ro->Release();
    }

    if (cursor)
    {
        cursor->Bind();
        glDrawElements(GL_LINES, cursor->GetIndexCount(), GL_UNSIGNED_INT, 0);
        cursor->Release();
    }
}



void GLWidget::UpdateTorusObjectTransform(QVector3D pos, QVector3D rot, QVector3D scale)
{
    /*torus->Position = pos;
    torus->Rotation = rot;
    torus->Scale = scale;

    makeCurrent();
    shader->SetUniform("u_MVP.Model", torus->GetModelMatrix());
    update();*/
}

GLWidget::~GLWidget()
{
    makeCurrent();

    for (IRenderableObject* ro : renderableObjects)
    {
        delete ro;
    }
}

void GLWidget::UpdateTorusObjectParameters(float R, float r, int Rdensity, int rdensity)
{
    /*torus->SetBiggerRadius(R);
    torus->SetSmallerRadius(r);
    torus->SetBiggerRadiusDensity(Rdensity);
    torus->SetSmallerRadiusDensity(rdensity);

    auto edges = torus->GenerateTopologyEdges();
    auto vertices = torus->GenerateGeometryVertices();

    makeCurrent();
    vb->bind();
    vb->allocate(vertices.data(), sizeof(float) * vertices.size());
    vb->release();
    va->bind();
    ib->bind();
    ib->allocate(edges.data(), sizeof(int) * edges.size());
    va->release();
    update();*/
}

void GLWidget::UpdateCameraSlot(std::shared_ptr<CameraUpdateEvent> event)
{
    makeCurrent();
    shader->SetUniform("u_MVP.View", event->NewViewMatrix);
    shader2->SetUniform("u_MVP.View", event->NewViewMatrix);
    update();
}

void GLWidget::MouseRaycastSlot(std::shared_ptr<SceneMouseClickEvent> event)
{
    QVector4D plain = controls->Camera->GetCenterViewPlain();
    QVector4D raycastDirection = (event->ClickViewPointFar - event->ClickViewPointNear).toVector4D();
    QVector4D raycastStart = controls->Camera->GetPosition().toVector4D();
    raycastStart.setW(1.0f);

    float t = -QVector4D::dotProduct(plain, raycastStart) / QVector4D::dotProduct(plain, raycastDirection);

    QVector3D clickPoint = (raycastDirection * t + raycastStart).toVector3D();

    qDebug() << "CenterViewPlain:" << plain;
    qDebug() << "ClickPoint:" << clickPoint;

    if (cursor)
        cursor->Position = clickPoint;
    else
    {
        makeCurrent();
        cursor = std::make_unique<CursorObject>(clickPoint, shader2);
    }

    update();
}
