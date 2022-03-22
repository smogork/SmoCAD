//
// Created by ksm on 3/22/22.
//

#include "PointObject.h"

void PointObject::CreateBuffers()
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

    va->create();
    Shader->Bind();
    va->bind();
    vb->bind();
    ib->bind();

    int stride = 3 * sizeof(float); //only position on 3 floats
    //[TODO] Dodac klase opisujaca uklad buforow
    Shader->GetRawProgram()->enableAttributeArray(0);
    Shader->GetRawProgram()->setAttributeBuffer(0, GL_FLOAT, 0, 3, stride);

    va->release();

    vb->release();
    ib->release();
    Shader->Release();
}

PointObject::PointObject(QVector3D pos, std::shared_ptr<ShaderWrapper> shader)
    : TransformableObject(pos), IRenderableObject(shader)
{
    vb = std::make_unique<QOpenGLBuffer>(QOpenGLBuffer::VertexBuffer);
    ib = std::make_unique<QOpenGLBuffer>(QOpenGLBuffer::IndexBuffer);
    CreateBuffers();
}

PointObject::~PointObject()
{
    vb->destroy();
    ib->destroy();
}

int PointObject::GetIndexCount()
{
    return 1;
}

void PointObject::Bind()
{
    Shader->SetUniform("u_MVP.Model", GetModelMatrix());

    IRenderableObject::Bind();
}

std::vector<float> PointObject::GenerateGeometryVertices()
{
    return {0.0f, 0.0f, 0.0f};
}

std::vector<int> PointObject::GenerateTopologyEdges()
{
    return { 0 };
}
