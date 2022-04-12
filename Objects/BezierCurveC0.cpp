//
// Created by ksm on 3/27/22.
//

#include <cfloat>
#include "BezierCurveC0.h"

std::vector<float> BezierCurveC0::GenerateGeometryVertices()
{
    std::vector<float> res (4 * controlPoints.size());

    int i = 0;
    for (PointObject* p : controlPoints)
    {
        res[4 * i] = p->Position.x();
        res[4 * i + 1] = p->Position.y();
        res[4 * i + 2] = p->Position.z();
        res[4 * i + 3] = 1.0f;
        i++;
    }

    return res;
}

std::vector<int> BezierCurveC0::GenerateTopologyEdges()
{
    int groups = std::ceil(controlPoints.size() / 3.0f);
    std::vector<int> res (4 * groups);

    for (int i = 0; i < groups; ++i)
    {
        res[4 * i] = 3 * i;
        res[4 * i + 1] = 3 * i + 1;
        res[4 * i + 2] = 3 * i + 2;
        res[4 * i + 3] = 3 * i + 3;
    }

    return res;
}

void BezierCurveC0::CreateBuffers()
{
    vb->create();
    vb->bind();
    vb->setUsagePattern(QOpenGLBuffer::StaticDraw);
    auto vertices = GenerateGeometryVertices();
    vb->allocate(vertices.data(), sizeof(float) * vertices.size());
    vb->release();

    ib->create();
    ib->bind();
    ib->setUsagePattern(QOpenGLBuffer::StaticDraw);
    auto edges = GenerateTopologyEdges();
    ib->allocate(edges.data(), sizeof(int) * edges.size());
    ib->release();

    //To jest fake m_shader, który oszukuje qt wrapper na opengl
    //atrybuty nie są zwaizane ze stane shadera, tylko vertex array.
    //Wiec tworzymy sztuczny obiekt shadera aby zadeklarowac w VA uklad atrybutow.
    QOpenGLShaderProgram prog;
    prog.create();
    va->create();
    prog.bind();
    va->bind();
    vb->bind();

    int stride = 4 * sizeof(float); //only position on 3 floats
    prog.enableAttributeArray(0);
    prog.setAttributeBuffer(0, GL_FLOAT, 0, 4, stride);

    ib->bind();
    va->release();

    vb->release();
    ib->release();
    prog.release();
}

int BezierCurveC0::GetIndexCount()
{
    return 4 * std::ceil(controlPoints.size() / 3.0f);
}

void BezierCurveC0::Bind(ShaderWrapper *shader)
{
    shader->SetUniform("u_MVP.Model", QMatrix4x4());
    if (Selected)
        shader->SetUniform("u_ObjectColor", QVector4D(0.8f, 0.8f, 0.8f, 1.0f));
    else
        shader->SetUniform("u_ObjectColor", QVector4D(1.0f, 0.5f, 0.2f, 1.0f));

    IRenderableObject::Bind(shader);
}

void BezierCurveC0::DefineBuffers()
{
    CreateBuffers();

    IRenderableObject::DefineBuffers();
}

BezierCurveC0::BezierCurveC0() : IRenderableObject(QVector3D())
{
    vb = std::make_unique<QOpenGLBuffer>(QOpenGLBuffer::VertexBuffer);
    ib = std::make_unique<QOpenGLBuffer>(QOpenGLBuffer::IndexBuffer);
}

BezierCurveC0::~BezierCurveC0()
{
    vb->destroy();
    ib->destroy();
}

void BezierCurveC0::AddControlPoint(PointObject *point)
{
    if (!point)
        return;

    controlPoints.push_back(point);
    buffersToUpdate = true;
}

void BezierCurveC0::RemovePoint(PointObject *point)
{
    if (!point)
        return;

    if (controlPoints.remove(point))
        buffersToUpdate = true;
}

void BezierCurveC0::UpdateBuffers()
{
    if (!AreBuffersCreated())
        return;

    auto edges = GenerateTopologyEdges();
    auto vertices = GenerateGeometryVertices();

    vb->bind();
    vb->allocate(vertices.data(), sizeof(float) * vertices.size());
    vb->release();

    ib->bind();
    ib->allocate(edges.data(), sizeof(int) * edges.size());

    IRenderableObject::UpdateBuffers();
}

void BezierCurveC0::onPointChanged(std::shared_ptr<PointObjectChangedEvent> event)
{
    auto found = std::find_if(controlPoints.begin(), controlPoints.end(),
                              [&](PointObject* &item)
                              {
                                  return event->ChangedPoint == item;
                              });

    if (found != controlPoints.end())
    {
        if (event->IsRemoved)
            RemovePoint(event->ChangedPoint);
        buffersToUpdate = true;
    }
}

int BezierCurveC0::CalculateDrawableChunks(QMatrix4x4 proj, QMatrix4x4 view, QSize viewport)
{
    float maxX = FLT_MIN, maxY = FLT_MIN, minX = FLT_MAX, minY = FLT_MAX;
    for (PointObject* p : controlPoints)
    {
        QVector3D screenPoint = p->Position.project(view, proj, QRect(QPoint(), viewport));

        maxX = std::max(maxX, screenPoint.x());
        maxY = std::max(maxY, screenPoint.y());
        minX = std::min(minX, screenPoint.x());
        minY = std::min(minY, screenPoint.y());
    }

    int length = std::max(maxX - minX, maxY - minY);
    length /= 4;
    int res = std::min(std::max(4, length), 64);

    return res;
}
