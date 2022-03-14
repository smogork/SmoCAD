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
}

void GLWidget::initializeGL()
{
    initializeOpenGLFunctions();

    UpdateProjectionMatrix((float)(size().width()) / (float)(size().height()));

    shader = std::make_unique<QOpenGLShaderProgram>();
    if (!shader->addShaderFromSourceFile(QOpenGLShader::Vertex, "test.vert")) qDebug() << shader->log();
    if (!shader->addShaderFromSourceFile(QOpenGLShader::Fragment, "test.frag")) qDebug() << shader->log();
    if (!shader->link()) qDebug() << shader->log();

    //cube = std::make_unique<CubeObject>(QVector3D());
    torus = std::make_unique<TorusObject>(QVector3D(), 5, 2, 128, 48);

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
    ib->allocate(edges.data(), sizeof(float) * edges.size());

    int stride = 3 * sizeof(float); //only position on 3 floats

    shader->enableAttributeArray(0);
    shader->setAttributeBuffer(0, GL_FLOAT, 0, 3, stride);

    camera = std::make_unique<OrbitalCamera>(torus->Position, 5.0f);

    //[TODO] dodac wrapper na shadery aby nie trzeba bylo pamietac specjalnie numerkow uniformow
    int u_viewMatrixLoc = shader->uniformLocation("u_MVP.View");
    int u_projMatrixLoc = shader->uniformLocation("u_MVP.Projection");
    int u_modelMatrixLoc = shader->uniformLocation("u_MVP.Model");
    shader->bind();
    shader->setUniformValue(u_viewMatrixLoc, camera->GetViewMatrix());
    shader->setUniformValue(u_projMatrixLoc, projectionMatrix);
    shader->setUniformValue(u_modelMatrixLoc, torus->GetModelMatrix());
    shader->release();
}

void GLWidget::resizeGL(int w, int h)
{
    QOpenGLWidget::resizeGL(w,h);

    UpdateProjectionMatrix((float)w / (float)h);
    int u_projMatrixLoc = shader->uniformLocation("u_MVP.Projection");
    shader->bind();
    shader->setUniformValue(u_projMatrixLoc, projectionMatrix);
}

void GLWidget::paintGL()
{
    // set the background color = clear color
    glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // use our shader program
    shader->bind();
    // bind the vertex array object, which in turn binds the vertex buffer object and
    // sets the attribute buffer in the OpenGL context
    va->bind();
    // For old Intel drivers you may need to explicitely re-bind the index buffer, because
    // these drivers do not remember the binding-state of the index/element-buffer in the VAO
    //	m_indexBufferObject.bind();

    // now draw the two triangles via index drawing
    // - GL_TRIANGLES - draw individual triangles via elements
    glDrawElements(GL_LINES, torus->GetIndexCount(), GL_UNSIGNED_INT, 0);


    // finally release VAO again (not really necessary, just for completeness)
    va->release();

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

void GLWidget::mouseReleaseEvent(QMouseEvent *event)
{
    int id = translateMouseButton(event->button());
    if (id != UNDEFINED_ID)
        mousePresses[id] = false;

    if (!(mousePresses[LMOUSE_ID] or mousePresses[MMOUSE_ID] or mousePresses[RMOUSE_ID]))
    {
        lastMousePos.setX(0);
        lastMousePos.setY(0);
    }
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    int id = translateMouseButton(event->button());
    if (id != UNDEFINED_ID)
        mousePresses[id] = true;
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (lastMousePos.isNull())
    {
        lastMousePos = event->pos();
        return;
    }
    QVector2D dMove = QVector2D(event->pos() - lastMousePos);
    lastMousePos = event->pos();

    if (mousePresses[MMOUSE_ID])
    {
        if (mousePresses[RMOUSE_ID])
        {
            camera->RotateAroundCenter(dMove.x() * ROTATE_SENSITIVITY, dMove.y() * ROTATE_SENSITIVITY);
        }
        else
        {
            camera->MoveRight(-dMove.x() * MOVE_SENSITIVITY);
            camera->MoveUp(dMove.y() * MOVE_SENSITIVITY);
        }

        int u_viewMatrixLoc = shader->uniformLocation("u_MVP.View");
        shader->bind();
        shader->setUniformValue(u_viewMatrixLoc, camera->GetViewMatrix());
        update();
    }
}

void GLWidget::wheelEvent(QWheelEvent *event)
{
    camera->ChangePivotLength(-event->angleDelta().y() * ZOOM_SENSITIVITY);

    int u_viewMatrixLoc = shader->uniformLocation("u_MVP.View");
    shader->bind();
    shader->setUniformValue(u_viewMatrixLoc, camera->GetViewMatrix());
    update();
}

void GLWidget::keyPressEvent(QKeyEvent *event)
{
    QWidget::keyPressEvent(event);
}

void GLWidget::keyReleaseEvent(QKeyEvent *event)
{
    QWidget::keyReleaseEvent(event);
}

int GLWidget::translateMouseButton(Qt::MouseButton button)
{
    switch(button)
    {
        case Qt::MouseButton::LeftButton:
            return LMOUSE_ID;
        case Qt::MouseButton::MiddleButton:
            return MMOUSE_ID;
        case Qt::MouseButton::RightButton:
            return RMOUSE_ID;
        default:
            return UNDEFINED_ID;
    }
}
