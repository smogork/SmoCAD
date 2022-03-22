//
// Created by ksm on 3/21/22.
//

#include "CursorObject.h"

std::vector<float> CursorObject::GenerateGeometryVertices()
{
    return {
        0.0f, 0.0f, 0.0f, 1.0f, 0.5f, 0.2f,
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

    va->create();
    Shader->Bind();
    va->bind();
    vb->bind();

    int stride = 3 * sizeof(float); //only position on 3 floats
    //[TODO] Dodac klase opisujaca uklad buforow
    Shader->GetRawProgram()->enableAttributeArray(0);
    Shader->GetRawProgram()->setAttributeBuffer(0, GL_FLOAT, 0, 3, 2*stride);
    Shader->GetRawProgram()->enableAttributeArray(1);
    Shader->GetRawProgram()->setAttributeBuffer(1, GL_FLOAT, 3 * sizeof(float), 3, 2*stride);

    ib->bind();
    va->release();

    vb->release();
    ib->release();
    Shader->Release();
}

CursorObject::CursorObject(QVector3D pos, std::shared_ptr<ShaderWrapper> shader)
: IRenderableObject(pos, shader)
{
    vb = std::make_unique<QOpenGLBuffer>(QOpenGLBuffer::VertexBuffer);
    ib = std::make_unique<QOpenGLBuffer>(QOpenGLBuffer::IndexBuffer);
    CreateBuffers();
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

void CursorObject::Bind()
{
    Shader->SetUniform("u_MVP.Model", GetModelMatrix());

    IRenderableObject::Bind();
}
