#include <QOpenGLFunctions_4_4_Core>
#include "glwidget.h"
#include "Scene/Systems/DrawingSystem.h"
#include "Scene/SceneECS.h"
#include "Controls/EntityContextMenu.h"

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
    this->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &GLWidget::customContextMenuRequested, this,
            &GLWidget::showObjectListContextMenu);
}

void GLWidget::initializeGL()
{
    initializeOpenGLFunctions();
    auto gl = context()->functions();


    gl->glEnable(GL_PROGRAM_POINT_SIZE);
    gl->glEnable(GL_BLEND);
    gl->glEnable(GL_DEPTH_TEST);
    gl->glEnable(GL_CULL_FACE);
    gl->glCullFace(GL_BACK);
    
    LoadShaders();
    Renderer::UpdateShaders();
    
    if (auto scene = SceneECS::Instance().lock())
    {
        scene->InitializeScene();
        qDebug().noquote()  << scene->DebugSystemReport();
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
    auto gl = context()->functions();

    // set the background color = clear color
    gl->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    gl->glColorMask(true, true, true, true);
    gl->glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
    gl->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
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
        scene->RemoveUniqueObjects();
        scene->RemoveObjectsFromScene();
        scene->ClearSystems();
        qDebug().noquote()  << scene->DebugSystemReport();
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
                        std::make_shared<ShaderWrapper>("Shaders/uniform_color.vert",
                                                        "Shaders/simple_color.frag"));
    Renderer::AddShader(TORUS_SHADER,
                        std::make_shared<ShaderWrapper>("Shaders/torus.vert",
                                                        "Shaders/torus.frag"));
    Renderer::AddShader(CURSOR_SHADER,
                        std::make_shared<ShaderWrapper>("Shaders/buffer_color.vert",
                                                        "Shaders/simple_color.frag"));
    Renderer::AddShader(BEZIER_SHADER,
                        std::make_shared<ShaderWrapper>("Shaders/Curves/bezier.vert", "Shaders/Curves/bezier.frag",
                                                        "Shaders/Curves/bezier.tess", "Shaders/Curves/bezier.eval"));
    Renderer::AddShader(BEZIERC2_SHADER,
                        std::make_shared<ShaderWrapper>("Shaders/Curves/bezierC2.vert", "Shaders/Curves/bezierC2.frag",
                                                        "Shaders/Curves/bezierC2.tess",
                                                        "Shaders/Curves/bezierC2.eval"));
    Renderer::AddShader(PLANE_SHADER,
                        std::make_shared<ShaderWrapper>("Shaders/Planes/plane.vert", "Shaders/Planes/plane.frag",
                                                        "Shaders/Planes/plane.tess", "Shaders/Planes/plane.eval"));
    Renderer::AddShader(PLANEC2_SHADER,
                        std::make_shared<ShaderWrapper>("Shaders/Planes/planeC2.vert", "Shaders/Planes/planeC2.frag",
                                                        "Shaders/Planes/planeC2.tess", "Shaders/Planes/planeC2.eval"));
    Renderer::AddShader(SELECT_RECT_SHADER,
                        std::make_shared<ShaderWrapper>("Shaders/select_rect.vert", "Shaders/select_rect.frag"));
    Renderer::AddShader(FILL_PLANE_SHADER,
                        std::make_shared<ShaderWrapper>("Shaders/Planes/gregory_plane.vert",
                                                        "Shaders/Planes/gregory_plane.frag",
                                                        "Shaders/Planes/gregory_plane.tess",
                                                        "Shaders/Planes/gregory_plane.eval"));
    Renderer::AddShader(BLOCK_LOWERWALL_SHADER, std::make_shared<ShaderWrapper>("Shaders/Simulator3C/lower_wall.vert",
                                                                                "Shaders/Simulator3C/lower_wall.frag"));
    Renderer::AddShader(BLOCK_SIDEWALL_SHADER, std::make_shared<ShaderWrapper>("Shaders/Simulator3C/side_wall.vert",
                                                                                "Shaders/Simulator3C/side_wall.frag"));
    Renderer::AddShader(BLOCK_UPPERWALL_SHADER, std::make_shared<ShaderWrapper>("Shaders/Simulator3C/upper_wall.vert",
                                                                               "Shaders/Simulator3C/upper_wall.frag"));
    Renderer::AddShader(SIMULATOR_CUTTER_SHADER, std::make_shared<ShaderWrapper>("Shaders/Simulator3C/cutter.vert",
                                                                                "Shaders/Simulator3C/cutter.frag"));
}

void GLWidget::RedrawScreen()
{
    update();
}

void GLWidget::showObjectListContextMenu(const QPoint &pos)
{
    QPoint globalPos = mapToGlobal(pos);
    
    if (auto scene = SceneECS::Instance().lock())
        if (auto elSys = scene->GetSystem<SceneElementSystem>().lock())
        {
            std::unique_ptr<QMenu> menu = EntityContextMenu::CreateMenuForScene();
            if (menu)
                menu->exec(globalPos);
        }
}

