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
    format.setVersion(4,4);
    setFormat(format);
}

void GLWidget::initializeGL()
{
    initializeOpenGLFunctions();

    glEnable(GL_PROGRAM_POINT_SIZE);
    //glEnable(GL_POINT_SMOOTH);

    shaders.push_back(std::make_shared<ShaderWrapper>("Shaders/uniform_color.vert", "Shaders/simple_color.frag"));//default
    shaders.push_back(std::make_shared<ShaderWrapper>("Shaders/buffer_color.vert", "Shaders/simple_color.frag"));//cursor
    shaders.push_back(std::make_shared<ShaderWrapper>("Shaders/bezier.vert", "Shaders/bezier.frag",
                                                      "Shaders/bezier.tess", "Shaders/bezier.eval"));//bezier

    InitializeUniforms();
}

void GLWidget::resizeGL(int w, int h)
{
    QOpenGLWidget::resizeGL(w,h);
    emit WidgetResized(QSize(w, h));

    auto proj = controls->viewport->UpdatePerspectiveMatrix(QSize(w, h));

    for (auto sh : shaders)
        sh->SetUniform("u_MVP.Projection", proj);

}

void GLWidget::paintGL()
{
    // set the background color = clear color
    glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //Objects on scene
    for (IRenderableObject* ro : scene->GetRenderableObjects())
        DrawRenderableObject(ro, shaders[DEFAULT_SHADER]);

    //Composite object
    const std::unique_ptr<CompositeObject>& composite = scene->GetCompositeObject();
    if (composite)
    {
        for (CompositeObject::CompositeTransform &o: composite->GetObjects())
        {
            QMatrix4x4 model = composite->GetModelMatrix() * o.dTransform.GetModelMatrix();
            DrawRenderableObject(o.Object, shaders[DEFAULT_SHADER], [model](ShaderWrapper* sh) {
                sh->SetUniform("u_MVP.Model", model);
            });
        }
        DrawRenderableObject(composite->GetCenterCursor().get(), shaders[CURSOR_SHADER]);
    }

    //User cursor
    DrawRenderableObject(scene->GetCursorObject().get(), shaders[CURSOR_SHADER]);
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

        if (ro->AreBuffersToUpdate())
            ro->UpdateBuffers();

        ro->Bind(shader.get());
        if (uniformOverrides)
        {
            uniformOverrides(shader.get());
            shader->Bind();
        }
        glDrawElements(ro->GetDrawType(), ro->GetIndexCount(), GL_UNSIGNED_INT, 0);
        ro->Release(shader.get());
    }
}

void GLWidget::UpdateCameraSlot(std::shared_ptr<CameraUpdateEvent> event)
{
    makeCurrent();
    for (auto sh : shaders)
        sh->SetUniform("u_MVP.View", event->NewViewMatrix);
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
    for (auto sh : shaders)
    {
        sh->SetUniform("u_MVP.View", controls->Camera->GetViewMatrix());
        sh->SetUniform("u_MVP.Projection", controls->viewport->GetProjectionMatrix());
    }
}
