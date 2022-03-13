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

    cube = std::make_unique<CubeObject>(QVector3D());

    vb = std::make_unique<QOpenGLBuffer>(QOpenGLBuffer::VertexBuffer);
    vb->create();
    vb->setUsagePattern(QOpenGLBuffer::StaticDraw);
    vb->bind();
    auto vertices = cube->GenerateGeometryVertices();
    vb->allocate(vertices.data(), sizeof(float) * vertices.size());

    va = std::make_unique<QOpenGLVertexArrayObject>();
    bool test = va->create();
    va->bind();

    ib = std::make_unique<QOpenGLBuffer>(QOpenGLBuffer::IndexBuffer);
    ib->create();
    ib->setUsagePattern(QOpenGLBuffer::StaticDraw);
    ib->bind();
    auto edges = cube->GenerateTopologyEdges();
    ib->allocate(edges.data(), sizeof(float) * edges.size());

    int stride = 3 * sizeof(float); //only position on 3 floats

    shader->enableAttributeArray(0);
    shader->setAttributeBuffer(0, GL_FLOAT, 0, 3, stride);

    Camera = std::make_unique<OrbitalCamera>(cube->Position, 5.0f);

    //[TODO] dodac wrapper na shadery aby nie trzeba bylo pamietac specjalnie numerkow uniformow
    int u_viewMatrixLoc = shader->uniformLocation("u_MVP.View");
    int u_projMatrixLoc = shader->uniformLocation("u_MVP.Projection");
    int u_modelMatrixLoc = shader->uniformLocation("u_MVP.Model");
    auto m = cube->GetModelMatrix();
    auto v = Camera->GetViewMatrix();
    //auto v = QMatrix4x4();
    auto p = projectionMatrix;
    //auto p = QMatrix4x4();
    shader->bind();
    shader->setUniformValue(u_viewMatrixLoc, v);
    shader->setUniformValue(u_projMatrixLoc, p);
    shader->setUniformValue(u_modelMatrixLoc, m);
    shader->release();
}

void GLWidget::resizeGL(int w, int h)
{
    QOpenGLWidget::resizeGL(w,h);

    UpdateProjectionMatrix((float)w / (float)h);
    int u_projMatrixLoc = shader->uniformLocation("u_MVP.Projection");
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
    glDrawElements(GL_LINES, cube->GetIndexCount(), GL_UNSIGNED_INT, 0);


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

    makeCurrent();

}
