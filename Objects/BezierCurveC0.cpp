//
// Created by ksm on 3/27/22.
//

#include "BezierCurveC0.h"

std::vector<float> BezierCurveC0::GenerateGeometryVertices()
{
    return {
            0.0f, 0.0f, 0.0f,
            1.0f, 0.0f, 0.0f,
            2.0f, 0.0f, 1.0f,
            2.0f, 1.0f, 1.0f,
    };
}

std::vector<int> BezierCurveC0::GenerateTopologyEdges()
{
    return {
        0, 1, 2, 3
    };
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

    //To jest fake shader, który oszukuje qt wrapper na opengl
    //atrybuty nie są zwaizane ze stane shadera, tylko vertex array.
    //Wiec tworzymy sztuczny obiekt shadera aby zadeklarowac w VA uklad atrybutow.
    QOpenGLShaderProgram prog;
    prog.create();
    va->create();
    prog.bind();
    va->bind();
    vb->bind();

    int stride = 3 * sizeof(float); //only position on 3 floats
    //[TODO] Dodac klase opisujaca uklad buforow
    prog.enableAttributeArray(0);
    prog.setAttributeBuffer(0, GL_FLOAT, 0, 3, stride);

    ib->bind();
    va->release();

    vb->release();
    ib->release();
    prog.release();
}

int BezierCurveC0::GetIndexCount()
{
    return 4;
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
