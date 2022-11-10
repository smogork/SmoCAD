#include <QOpenGLFunctions_4_4_Core>
#include <memory>
#include <QOpenGLTexture>
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
    format.setOption(QSurfaceFormat::DebugContext);
    setFormat(format);
    makeCurrent();

    m_logger = std::make_unique<QOpenGLDebugLogger>(this);


    QObject::connect(m_logger.get(), &QOpenGLDebugLogger::messageLogged,
                     this, &GLWidget::OnOpenGLError);
    QObject::connect(&Renderer::controller, &InputController::CameraUpdated,
                     this, &GLWidget::UpdateCameraSlot);
    this->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &GLWidget::customContextMenuRequested, this,
            &GLWidget::showObjectListContextMenu);

}

void GLWidget::initializeGL()
{
    initializeOpenGLFunctions();

    m_logger->initialize();
    m_logger->startLogging();

    glEnable(GL_PROGRAM_POINT_SIZE);
    glEnable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE);
    //glCullFace(GL_BACK);

    LoadShaders();
    Renderer::UpdateShaders();

    if (auto scene = SceneECS::Instance().lock())
    {
        scene->InitializeScene();
        qDebug().noquote() << scene->DebugSystemReport();
    }

    m_isInitialized = true;
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
    //auto gl = (QOpenGLFunctions_4_4_Core*)this;

    // set the background color = clear color
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColorMask(true, true, true, true);
    glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (auto scene = SceneECS::Instance().lock())
    {
        if (auto dSystem = scene->GetSystem<DrawingSystem>().lock())
        {
            dSystem->Render(context());
        }
    }
}

void GLWidget::DrawOffscreen(QSize bufferSize, std::function<void(QOpenGLContext* context)> renderFunction)
{
    //the context should be valid. make sure it is current for painting
    makeCurrent();
    if (!m_isInitialized)
    {
        initializeGL();
        //resizeGL(width(), height());
    }
    if (!m_fbo || m_fbo->width() != bufferSize.width() || m_fbo->height() != bufferSize.height())
    {
        //allocate additional? FBO for rendering or resize it if widget size changed
        m_fbo.reset();
        QOpenGLFramebufferObjectFormat format;
        format.setAttachment(QOpenGLFramebufferObject::Depth);
        m_fbo = std::make_unique<QOpenGLFramebufferObject>(bufferSize.width(), bufferSize.height(), format);
        glViewport(0, 0, bufferSize.width(), bufferSize.height());
        //resizeGL(width(), height());
    }

    //GLuint fbo = m_fbo->handle();

    QOpenGLTexture depthTex(QOpenGLTexture::Target::Target2D);//TODO: wyciagnac teksture na zewnatrz
    //depthTex.setMinMagFilters(QOpenGLTexture::Filter::LinearMipMapLinear, QOpenGLTexture::Filter::NearestMipMapLinear);
    depthTex.create();

    depthTex.setSize(bufferSize.width(), bufferSize.height());
    depthTex.setFormat(QOpenGLTexture::D32F);
    depthTex.setAutoMipMapGenerationEnabled(false);
    depthTex.setMipBaseLevel(0);

    depthTex.setWrapMode(QOpenGLTexture::ClampToEdge);
    depthTex.allocateStorage(QOpenGLTexture::Depth, QOpenGLTexture::Float32);
    //depthTex.setDepthStencilMode(QOpenGLTexture::DepthMode);
    m_fbo->bind();
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,  depthTex.textureId(), 0);

    //#2 WORKS: bind FBO and render stuff with paintGL() call

    if (renderFunction)
        renderFunction(context());
    else
        paintGL();

    //You could now grab the content of the framebuffer we've rendered to
    QImage image2 = m_fbo->toImage();
    image2.save(QString("offscreenFB.png"));
    m_fbo->release();
    depthTex.destroy();//[TODO] Do wyrzucenia
    //#2 --------------------------------------------------------------

    //bind default framebuffer again. not sure if this necessary
    //and isn't supposed to use defaultFramebuffer()...
    m_fbo->bindDefault();
    //resizeGL(width(), height());
    doneCurrent();
}

GLWidget::~GLWidget()
{
    makeCurrent();

    if (auto scene = SceneECS::Instance().lock())
    {
        scene->RemoveUniqueObjects();
        scene->RemoveObjectsFromScene();
        scene->ClearSystems();
        qDebug().noquote() << scene->DebugSystemReport();
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

void GLWidget::OnOpenGLError(const QOpenGLDebugMessage &debugMessage)
{
    if (debugMessage.severity() < QOpenGLDebugMessage::NotificationSeverity)
        qDebug() << "OpenGL message: " << debugMessage;
}



