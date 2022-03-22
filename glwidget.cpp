#include <QOpenGLFunctions>
#include "glwidget.h"
#include "Objects/CursorObject.h"
#include "Objects/PointObject.h"

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

    glEnable(GL_PROGRAM_POINT_SIZE);
    //glEnable(GL_POINT_SMOOTH);

    shader = std::make_shared<ShaderWrapper>("Shaders/uniform_color.vert", "Shaders/simple_color.frag");
    shader2 = std::make_shared<ShaderWrapper>("Shaders/buffer_color.vert", "Shaders/simple_color.frag");

    renderableObjects.push_back(new CubeObject(QVector3D(0.0f, 5.0f, 5.0f)));
    renderableObjects.push_back(new CubeObject(QVector3D(0.0f, 0.0f, 5.0f)));
    renderableObjects.push_back(new PointObject(QVector3D(0.0f, 0.0f, 0.0f)));
    renderableObjects.push_back(new PointObject(QVector3D(1.0f, 0.0f, 0.0f)));
    renderableObjects.push_back(new PointObject(QVector3D(2.0f, 0.0f, 1.0f)));
    renderableObjects.push_back(new TorusObject(QVector3D(5.0f, 0.0f, 10.0f), 5, 1, 36, 18));

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
        DrawRenderableObject(ro, shader);

    DrawRenderableObject(cursor.get(), shader2);
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

    float t = -QVector4D::dotProduct(plain, event->RaycastStart) /
            QVector4D::dotProduct(plain, event->RaycastDirection);

    QVector3D clickPoint = (event->RaycastDirection * t + event->RaycastStart).toVector3D();

    /*qDebug() << "CenterViewPlain:" << plain;
    qDebug() << "ClickPoint:" << clickPoint;*/

    if (cursor)
        cursor->Position = clickPoint;
    else
    {
        makeCurrent();
        cursor = std::make_unique<CursorObject>(clickPoint);
    }

    update();
}

void GLWidget::AddSelectableObject(IRenderableObject *ro)
{
    if (cursor)
    {
        ro->Position = cursor->Position;
        this->renderableObjects.push_back(ro);
        update();
    }
}

void GLWidget::DeleteSelectableObject(IRenderableObject *ro)
{
    if (ro != nullptr)
    {
        this->renderableObjects.remove(ro);

        makeCurrent();
        delete ro;
    }
}

void GLWidget::DrawRenderableObject(IRenderableObject *ro, std::shared_ptr<ShaderWrapper> shader)
{
    if (ro)
    {
        if (!ro->AreBuffersCreated())
            ro->DefineBuffers();

        ro->Bind(shader.get());
        glDrawElements(ro->GetDrawType(), ro->GetIndexCount(), GL_UNSIGNED_INT, 0);
        ro->Release(shader.get());
    }
}
