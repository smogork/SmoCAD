#include <QOpenGLFunctions>
#include "glwidget.h"
#include "Objects/CursorObject.h"
#include "Objects/PointObject.h"
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
}

void GLWidget::initializeGL()
{
    initializeOpenGLFunctions();

    glEnable(GL_PROGRAM_POINT_SIZE);
    //glEnable(GL_POINT_SMOOTH);

    LoadShaders();
    UpdateUniforms();
}

void GLWidget::resizeGL(int w, int h)
{
    QOpenGLWidget::resizeGL(w, h);
    auto s = QSize(w, h);
    controls->viewport->UpdatePerspectiveMatrix(s);
    UpdateUniforms();
    emit WidgetResized(s);
    //renderer->UpdateUniforms(controls);

}

void GLWidget::paintGL()
{
    // set the background color = clear color
    glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /*//Objects on scene
    for (IRenderableObject* ro : scene->GetRenderableObjects())
    {
        //[TODO] Poprawić aby obiekty IRenderable mialy opcje renderowania sie same
        if (dynamic_cast<BezierCurveC0*>(ro) == nullptr)
            DrawRenderableObject(ro, shaders[DEFAULT_SHADER]);
        else
            DrawBezier(dynamic_cast<BezierCurveC0*>(ro));
    }

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
    DrawRenderableObject(scene->GetCursorObject().get(), shaders[CURSOR_SHADER]);*/

    if (auto scene = SceneECS::Instance().lock())
    {
        if (auto dSystem = scene->GetSystem<DrawingSystem>().lock())
        {
            for (const std::weak_ptr<Drawing> &d: dSystem->GetComponents())
                if (auto obj = d.lock())
                {
                    //[TODO] wyprowadzic gdzies shadery
                    obj->AttachShader(shaders[DEFAULT]);
                    obj->Render(context());
                }
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
    }
}

void
GLWidget::DrawRenderableObject(IRenderableObject *ro, std::shared_ptr<ShaderWrapper> shader, const std::function<void(
        ShaderWrapper *)> &uniformOverrides)
{
    /*if (ro)
    {
        if (!ro->AreBuffersCreated())
            ro->DefineBuffers();

        if (ro->AreBuffersToUpdate())
            ro->UpdateBuffers();

        ro->Bind(m_shader.get());
        if (uniformOverrides)
        {
            uniformOverrides(m_shader.get());
            m_shader->Bind();
        }

        glDrawElements(ro->GetDrawType(), ro->GetIndexCount(), GL_UNSIGNED_INT, 0);
        ro->Release(m_shader.get());
    }*/
}

void GLWidget::UpdateCameraSlot(std::shared_ptr<CameraUpdateEvent> event)
{
    makeCurrent();
    UpdateUniforms();
    update();
}

void GLWidget::SetupSceneAndControls(std::shared_ptr<InputController> controler)
{
    this->controls = controler;

    QObject::connect(this->controls.get(), &InputController::CameraUpdated,
                     this, &GLWidget::UpdateCameraSlot);
}


void GLWidget::DrawBezier(BezierCurveC0 *bezier, const std::function<void(ShaderWrapper *)> &uniformOverrides)
{
    /*if (bezier)
    {
        if (!bezier->AreBuffersCreated())
            bezier->DefineBuffers();

        if (bezier->AreBuffersToUpdate())
            bezier->UpdateBuffers();

        if (scene->ShowBezeierPolygon)
        {
            bezier->Bind(shaders[DEFAULT_SHADER].get());
            shaders[DEFAULT_SHADER]->SetUniform("u_ObjectColor", QVector4D(0.0f, 0.7f, 0.9f, 1.0f));
            if (uniformOverrides)
            {
                uniformOverrides(shaders[DEFAULT_SHADER].get());
            }
            shaders[DEFAULT_SHADER]->Bind();

            glDrawArrays(GL_LINE_STRIP, 0, bezier->GetVertexCount());
        }

        bezier->Bind(shaders[BEZIER_SHADER].get());
        shaders[BEZIER_SHADER]->SetUniform("m_Chunks",
                                           bezier->CalculateDrawableChunks(controls->viewport->GetProjectionMatrix(), controls->Camera->GetViewMatrix(), controls->viewport->GetViewportSize()));
        if (uniformOverrides)
        {
            uniformOverrides(shaders[BEZIER_SHADER].get());
        }
        shaders[BEZIER_SHADER]->Bind();
        glDrawElements(bezier->GetDrawType(), bezier->GetIndexCount(), GL_UNSIGNED_INT, 0);


        bezier->Release(shaders[DEFAULT_SHADER].get());
    }*/
}

std::weak_ptr<ShaderWrapper> GLWidget::GetShader(SHADERS shNumber)
{
    if (shNumber >= SHADERS::SHADERS_COUNT)
        throw std::runtime_error(QString("m_shader number %1 unknown").arg(shNumber).toStdString());
    return shaders[shNumber];
}

void GLWidget::LoadShaders()
{
    shaders.push_back(
            std::make_shared<ShaderWrapper>("Shaders/uniform_color.vert", "Shaders/simple_color.frag"));//default
    shaders.push_back(
            std::make_shared<ShaderWrapper>("Shaders/buffer_color.vert", "Shaders/simple_color.frag"));//cursor
    shaders.push_back(std::make_shared<ShaderWrapper>("Shaders/bezier.vert", "Shaders/bezier.frag",
                                                      "Shaders/bezier.tess", "Shaders/bezier.eval"));//bezier
    shaders[SHADERS::BEZIER]->GetRawProgram()->setPatchVertexCount(4);
}

void GLWidget::UpdateUniforms()
{
    for (auto sh : shaders)
    {
        sh->SetUniform("u_MVP.View", controls->Camera->GetViewMatrix());
        sh->SetUniform("u_MVP.Projection", controls->viewport->GetProjectionMatrix());

        //qDebug() << "Camera space (0,0,0) " << controls->Camera->GetViewMatrix() * QVector4D(0, 0, 0, 1);
        //qDebug() << "NDC space (0,0,0) " << controls->viewport->GetProjectionMatrix() * controls->Camera->GetViewMatrix() * QVector4D(0, 0, 0, 1);
    }
}

std::vector<std::shared_ptr<ShaderWrapper>> GLWidget::GetShaders()
{
    return std::vector<std::shared_ptr<ShaderWrapper>>();
}
