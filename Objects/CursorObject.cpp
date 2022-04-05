//
// Created by ksm on 3/21/22.
//

#include "CursorObject.h"

std::vector<float> CursorObject::GenerateGeometryVertices()
{
    return {
        0.0f, 0.0f, 0.0f, 0.5f, 0.5f, 0.5f,
        1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        };
}

std::vector<int> CursorObject::GenerateTopologyEdges()
{
    return {
        0, 1,
        0, 2,
        0, 3,
    };
}

void CursorObject::CreateBuffers()
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

    int stride = 3 * sizeof(float); //only position on 3 floats
    prog.enableAttributeArray(0);
    prog.setAttributeBuffer(0, GL_FLOAT, 0, 3, 2*stride);
    prog.enableAttributeArray(1);
    prog.setAttributeBuffer(1, GL_FLOAT, 3 * sizeof(float), 3, 2*stride);

    ib->bind();
    va->release();

    vb->release();
    ib->release();
    prog.release();
}

CursorObject::CursorObject(QVector3D pos)
: IRenderableObject(pos)
{
    vb = std::make_unique<QOpenGLBuffer>(QOpenGLBuffer::VertexBuffer);
    ib = std::make_unique<QOpenGLBuffer>(QOpenGLBuffer::IndexBuffer);
}

CursorObject::~CursorObject()
{
    vb->destroy();
    ib->destroy();
}

int CursorObject::GetIndexCount()
{
    return 6;
}

void CursorObject::Bind(ShaderWrapper* shader)
{
    shader->SetUniform("u_MVP.Model", GetModelMatrix());

    IRenderableObject::Bind(shader);
}

void CursorObject::DefineBuffers()
{
    CreateBuffers();
    IRenderableObject::DefineBuffers();
}
