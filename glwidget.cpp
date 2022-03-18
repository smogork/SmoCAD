#include <QOpenGLFunctions>
#include "glwidget.h"

GLWidget::GLWidget(QWidget *pWidget)
    : QOpenGLWidget(pWidget)
{
    QSurfaceFormat format;
    format.setRenderableType(QSurfaceFormat::OpenGL);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setVersion(3,3);
    setFormat(format);

    this->controls = std::make_unique<InputController>(this);

    QObject::connect(this->controls.get(), &InputController::CameraUpdated,
                     this, &GLWidget::UpdateCameraSlot);
}

void GLWidget::initializeGL()
{
    initializeOpenGLFunctions();

    UpdateProjectionMatrix((float)(size().width()) / (float)(size().height()));

    shader = std::make_unique<ShaderWrapper>("test.vert", "test.frag");
    shader->Create();

    //cube = std::make_unique<CubeObject>(QVector3D());
    //[TODO] wydzielic jakos ten rysowany poza glWidget - tonie ma sensu aktualizaowanie tego wszytskiego wewnatrz tego widgetu
    torus = std::make_unique<TorusObject>(QVector3D(), 5, 1, 36, 18);

    vb = std::make_unique<QOpenGLBuffer>(QOpenGLBuffer::VertexBuffer);
    vb->create();
    vb->setUsagePattern(QOpenGLBuffer::StaticDraw);
    vb->bind();
    auto vertices = torus->GenerateGeometryVertices();
    vb->allocate(vertices.data(), sizeof(float) * vertices.size());

    va = std::make_unique<QOpenGLVertexArrayObject>();
    bool test = va->create();
    va->bind();

    ib = std::make_unique<QOpenGLBuffer>(QOpenGLBuffer::IndexBuffer);
    ib->create();
    ib->setUsagePattern(QOpenGLBuffer::StaticDraw);
    ib->bind();
    auto edges = torus->GenerateTopologyEdges();
    ib->allocate(edges.data(), sizeof(int) * edges.size());

    int stride = 3 * sizeof(float); //only position on 3 floats

    //[TODO] Dodac klase opisujaca uklad buforow
    shader->GetRawProgram()->enableAttributeArray(0);
    shader->GetRawProgram()->setAttributeBuffer(0, GL_FLOAT, 0, 3, stride);

    //camera = std::make_unique<OrbitalCamera>(torus->Position, 10.0f);

    //[TODO] dodac wrapper na shadery aby nie trzeba bylo pamietac specjalnie numerkow uniformow
    /*int u_viewMatrixLoc = shader->uniformLocation("u_MVP.View");
    int u_projMatrixLoc = shader->uniformLocation("u_MVP.Projection");
    int u_modelMatrixLoc = shader->uniformLocation("u_MVP.Model");
    shader->bind();
    shader->setUniformValue(u_viewMatrixLoc, controls->Camera->GetViewMatrix());
    shader->setUniformValue(u_projMatrixLoc, projectionMatrix);
    shader->setUniformValue(u_modelMatrixLoc, torus->GetModelMatrix());
    shader->release();*/

    shader->SetUniform("u_MVP.Model", torus->GetModelMatrix());
    shader->SetUniform("u_MVP.View", controls->Camera->GetViewMatrix());
    shader->SetUniform("u_MVP.Projection", projectionMatrix);
}

void GLWidget::resizeGL(int w, int h)
{
    QOpenGLWidget::resizeGL(w,h);

    UpdateProjectionMatrix((float)w / (float)h);
    //int u_projMatrixLoc = shader->uniformLocation("u_MVP.Projection");
    //shader->bind();
    shader->SetUniform("u_MVP.Projection", projectionMatrix);
}

void GLWidget::paintGL()
{
    // set the background color = clear color
    glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shader->Bind();
    va->bind();
    glDrawElements(GL_LINES, torus->GetIndexCount(), GL_UNSIGNED_INT, 0);
    va->release();

}

void GLWidget::UpdateTorusObjectTransform(QVector3D pos, QVector3D rot, QVector3D scale)
{
    torus->Position = pos;
    torus->Rotation = rot;
    torus->Scale = scale;

    makeCurrent();
    //shader->ind();
    //int u_modelMatrixLoc = shader->uniformLocation("u_MVP.Model");
    shader->SetUniform("u_MVP.Model", torus->GetModelMatrix());
    update();
}

GLWidget::~GLWidget()
{
    makeCurrent();

    va->destroy();
    vb->destroy();
    ib->destroy();
}

void GLWidget::UpdateProjectionMatrix(float aspectRatio)
{
    projectionMatrix.setToIdentity();
    projectionMatrix.perspective(fov, aspectRatio, 1.0f, 100.0f);
}

void GLWidget::UpdateTorusObjectParameters(float R, float r, int Rdensity, int rdensity)
{
    torus->SetBiggerRadius(R);
    torus->SetSmallerRadius(r);
    torus->SetBiggerRadiusDensity(Rdensity);
    torus->SetSmallerRadiusDensity(rdensity);

    auto edges = torus->GenerateTopologyEdges();
    auto vertices = torus->GenerateGeometryVertices();

    vb->bind();
    vb->allocate(vertices.data(), sizeof(float) * vertices.size());
    vb->release();
    va->bind();
    ib->bind();
    ib->allocate(edges.data(), sizeof(int) * edges.size());
    va->release();
    update();
}

void GLWidget::UpdateCameraSlot(std::shared_ptr<CameraMovementEvent> event)
{
    makeCurrent();
    //shader->bind();
    //int u_viewMatrixLoc = shader->uniformLocation("u_MVP.View");
    shader->SetUniform("u_MVP.View", event->NewViewMatrix);
    update();
}
