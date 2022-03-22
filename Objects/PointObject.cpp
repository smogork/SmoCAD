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

    QOpenGLShaderProgram prog;
    prog.create();
    va->create();
    prog.bind();
    va->bind();
    vb->bind();
    ib->bind();

    int stride = 3 * sizeof(float); //only position on 3 floats
    //[TODO] Dodac klase opisujaca uklad buforow
    prog.enableAttributeArray(0);
    prog.setAttributeBuffer(0, GL_FLOAT, 0, 3, stride);

    va->release();

    vb->release();
    ib->release();
    prog.release();
}

PointObject::PointObject(QVector3D pos)
    : IRenderableObject(pos)
{
    vb = std::make_unique<QOpenGLBuffer>(QOpenGLBuffer::VertexBuffer);
    ib = std::make_unique<QOpenGLBuffer>(QOpenGLBuffer::IndexBuffer);
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

void PointObject::Bind(ShaderWrapper* shader)
{
    shader->SetUniform("u_MVP.Model", GetModelMatrix());

    IRenderableObject::Bind(shader);
}

std::vector<float> PointObject::GenerateGeometryVertices()
{
    return {0.0f, 0.0f, 0.0f};
}

std::vector<int> PointObject::GenerateTopologyEdges()
{
    return { 0 };
}

void PointObject::DefineBuffers()
{
    CreateBuffers();
    IRenderableObject::DefineBuffers();
}
