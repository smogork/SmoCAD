//
// Created by ksm on 3/13/22.
//

#include "CubeObject.h"

CubeObject::CubeObject(QVector3D pos, std::shared_ptr<ShaderWrapper> shader)
    : IRenderableObject(pos, shader)
{
    vb = std::make_unique<QOpenGLBuffer>(QOpenGLBuffer::VertexBuffer);
    ib = std::make_unique<QOpenGLBuffer>(QOpenGLBuffer::IndexBuffer);
    CreateBuffers();
}

CubeObject::~CubeObject()
{
    vb->destroy();
    ib->destroy();
}

std::vector<float> CubeObject::GenerateGeometryVertices()
{
    return {
        +0.5f, +0.5f, +0.5f,  // front top right 0
        +0.5f, -0.5f, +0.5f,  // front bottom right 1
        -0.5f, -0.5f, +0.5f,  // front bottom left 2
        -0.5f, +0.5f, +0.5f,   // front top left 3
        +0.5f, +0.5f, -0.5f,  // back top right 4
        +0.5f, -0.5f, -0.5f,  // back bottom right 5
        -0.5f, -0.5f, -0.5f,  // back bottom left 6
        -0.5f, +0.5f, -0.5f  //back top left 7
};
}

std::vector<int> CubeObject::GenerateTopologyEdges()
{
    return {
        0, 1,
        1, 2,
        2, 3,
        0, 3,
        4, 5,
        5, 6,
        6, 7,
        4, 7,
        3, 7,
        2, 6,
        1, 5,
        0, 4,
};
}

void CubeObject::CreateBuffers()
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
    Shader->GetRawProgram()->setAttributeBuffer(0, GL_FLOAT, 0, 3, stride);

    ib->bind();
    va->release();

    vb->release();
    ib->release();
    Shader->Release();
}

int CubeObject::GetIndexCount()
{
    return 24;
}

void CubeObject::Bind()
{
    Shader->SetUniform("u_MVP.Model", GetModelMatrix());

    IRenderableObject::Bind();
}

