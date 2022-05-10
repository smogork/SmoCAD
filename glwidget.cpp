#include <QOpenGLFunctions>
#include "glwidget.h"
#include "Scene/Systems/DrawingSystem.h"
#include "Scene/SceneECS.h"

GLWidget::GLWidget(QWidget *pWidget)
        : QOpenGLWidget(pWidget)
{
    QSurfaceFormat format;
    format.setRenderableType(QSurfaceFormat::OpenGL);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setVersion(4, 4);
    setFormat(format);
    makeCurrent();

    QObject::connect(&Renderer::controller, &InputController::CameraUpdated,
                     this, &GLWidget::UpdateCameraSlot);
}

void GLWidget::initializeGL()
{
    initializeOpenGLFunctions();

    glEnable(GL_PROGRAM_POINT_SIZE);
    glEnable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);

    LoadShaders();
    Renderer::UpdateShaders();

    if (auto scene = SceneECS::Instance().lock())
    {
        scene->InitializeScene();
        qDebug() << scene->DebugSystemReport();
    }
}

void GLWidget::resizeGL(int w, int h)
{
    QOpenGLWidget::resizeGL(w, h);
    auto s = QSize(w, h);

    Renderer::controller.viewport->UpdatePerspectiveMatrix(s);
    Renderer::UpdateShaders();

    emit WidgetResized(s);
}

void GLWidget::paintGL()
{
    // set the background color = clear color
    glBlendFunc(GL_ONE, GL_ZERO);
    glColorMask(true, true, true, true);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (auto scene = SceneECS::Instance().lock())
    {
        if (auto dSystem = scene->GetSystem<DrawingSystem>().lock())
        {
            dSystem->Render(context());
        }
    }
}

GLWidget::~GLWidget()
{
    makeCurrent();

    if (auto scene = SceneECS::Instance().lock())
    {
        scene->RemoveObjectsFromScene();
        scene->RemoveUniqueObjects();
        scene->ClearSystems();
        qDebug() << scene->DebugSystemReport();
    }
}

void GLWidget::UpdateCameraSlot(std::shared_ptr<CameraUpdateEvent> event)
{
    makeCurrent();
    Renderer::UpdateShaders();
    update();
}

void GLWidget::LoadShaders()
{
    Renderer::AddShader(DEFAULT_SHADER,
                        std::make_shared<ShaderWrapper>("Shaders/uniform_color.vert", "Shaders/simple_color.frag"));//default
    Renderer::AddShader(CURSOR_SHADER,
                        std::make_shared<ShaderWrapper>("Shaders/buffer_color.vert", "Shaders/simple_color.frag"));//cursor
    Renderer::AddShader(BEZIER_SHADER,
                        std::make_shared<ShaderWrapper>("Shaders/bezier.vert", "Shaders/bezier.frag",
                                    "Shaders/bezier.tess", "Shaders/bezier.eval"));//bezier
    Renderer::GetShader(SHADERS::BEZIER_SHADER).lock()->GetRawProgram()->setPatchVertexCount(4);
}

void GLWidget::RedrawScreen()
{
    update();
}

