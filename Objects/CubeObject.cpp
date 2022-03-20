//
// Created by ksm on 3/13/22.
//

#include "CubeObject.h"

CubeObject::CubeObject(QVector3D pos): TransformableObject(pos)
{
    vb = std::make_unique<QOpenGLBuffer>(QOpenGLBuffer::VertexBuffer);
    ib = std::make_unique<QOpenGLBuffer>(QOpenGLBuffer::IndexBuffer);
    CreateBuffers();
}

CubeObject::~CubeObject()
{
    vb->destroy();
    ib->destroy();
    if (va->isCreated())
        va->destroy();
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

    va->create();
    va->bind();

    vb->create();
    vb->setUsagePattern(QOpenGLBuffer::StaticDraw);
    vb->bind();
    auto vertices = GenerateGeometryVertices();
    vb->allocate(vertices.data(), sizeof(float) * vertices.size());

    ib->create();
    ib->setUsagePattern(QOpenGLBuffer::DynamicDraw);
    ib->bind();
    auto edges = GenerateTopologyEdges();
    ib->allocate(edges.data(), sizeof(int) * edges.size());

    //va->release();
    //vb->release();
}

int CubeObject::GetIndexCount()
{
    return 24;
}
