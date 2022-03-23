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
}

void GLWidget::initializeGL()
{
    initializeOpenGLFunctions();

    glEnable(GL_PROGRAM_POINT_SIZE);
    //glEnable(GL_POINT_SMOOTH);

    shader = std::make_shared<ShaderWrapper>("Shaders/uniform_color.vert", "Shaders/simple_color.frag");
    shader2 = std::make_shared<ShaderWrapper>("Shaders/buffer_color.vert", "Shaders/simple_color.frag");

    InitializeUniforms();
}

void GLWidget::resizeGL(int w, int h)
{
    //[TODO] zglosic informacje o zmianie rozmiaru ekranu do spinów z pozycji kursora na ekranie
    QOpenGLWidget::resizeGL(w,h);

    auto proj = controls->viewport->UpdatePerspectiveMatrix(QSize(w, h));
    shader->SetUniform("u_MVP.Projection", proj);
    shader2->SetUniform("u_MVP.Projection", proj);
}

void GLWidget::paintGL()
{
    // set the background color = clear color
    glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //Objects on scene
    for (IRenderableObject* ro : scene->GetRenderableObjects())
        DrawRenderableObject(ro, shader);

    //Composite object
    const std::unique_ptr<CompositeObject>& composite = scene->GetCompositeObject();
    if (composite)
    {
        for (CompositeObject::CompositeTransform &o: composite->GetObjects())
        {
            DrawRenderableObject(o.Object, shader, [&](ShaderWrapper* sh) {
                sh->SetUniform("u_MVP.Model", composite->GetModelMatrix() * o.dTransform.GetModelMatrix());
            });
        }
        DrawRenderableObject(composite->GetCenterCursor().get(), shader2);
    }

    //User cursor
    DrawRenderableObject(scene->GetCursorObject().get(), shader2);
}

GLWidget::~GLWidget()
{
    makeCurrent();

    scene->ReleaseObjectsOnScene();
}

void GLWidget::DrawRenderableObject(IRenderableObject *ro, std::shared_ptr<ShaderWrapper> shader, const std::function< void(
        ShaderWrapper*)>& uniformOverrides)
{
    if (ro)
    {
        if (!ro->AreBuffersCreated())
            ro->DefineBuffers();

        ro->Bind(shader.get());
        if (uniformOverrides)
            uniformOverrides(shader.get());
        glDrawElements(ro->GetDrawType(), ro->GetIndexCount(), GL_UNSIGNED_INT, 0);
        ro->Release(shader.get());
    }
}

void GLWidget::UpdateCameraSlot(std::shared_ptr<CameraUpdateEvent> event)
{
    makeCurrent();
    shader->SetUniform("u_MVP.View", event->NewViewMatrix);
    shader2->SetUniform("u_MVP.View", event->NewViewMatrix);
    update();
}

void GLWidget::SetupSceneAndControls(std::shared_ptr<InputController> controler, std::shared_ptr<SceneModel> model)
{
    this->controls = controler;
    this->scene = model;

    QObject::connect(this->controls.get(), &InputController::CameraUpdated,
                     this, &GLWidget::UpdateCameraSlot);
}

void GLWidget::InitializeUniforms()
{
    shader->SetUniform("u_MVP.View", controls->Camera->GetViewMatrix());
    shader->SetUniform("u_MVP.Projection", controls->viewport->GetProjectionMatrix());

    shader2->SetUniform("u_MVP.View", controls->Camera->GetViewMatrix());
    shader2->SetUniform("u_MVP.Projection", controls->viewport->GetProjectionMatrix());
}
