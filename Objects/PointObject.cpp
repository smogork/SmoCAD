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
    if (Selected)
        shader->SetUniform("u_ObjectColor", QVector4D(0.8f, 0.8f, 0.8f, 1.0f));
    else
        shader->SetUniform("u_ObjectColor", QVector4D(1.0f, 0.5f, 0.2f, 1.0f));

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

float PointObject::TestAgainstRaycast(QVector4D raycastStart, QVector4D raycastDirection)
{
    QMatrix4x4 sphereMatrix;
    sphereMatrix(3,3) = -raycastSphereR * raycastSphereR;
    QMatrix4x4 model;
    model.translate(Position.x(), Position.y(), Position.z());
    QMatrix4x4 reverseModel = model.inverted();

    sphereMatrix = reverseModel.transposed() * sphereMatrix * reverseModel;

    float a = QVector4D::dotProduct(raycastDirection * sphereMatrix, raycastDirection);
    float b = 2 * QVector4D::dotProduct(raycastDirection * sphereMatrix, raycastStart);
    float c = QVector4D::dotProduct(raycastStart  * sphereMatrix, raycastStart);

    float delta = b * b - 4 * a * c;
    if (delta < 0.0f)
        return NAN;

    float deltasq = std::sqrt(delta);
    float t1 = (-b + deltasq) / (2.0f * a);
    float t2 = (-b - deltasq) / (2.0f * a);

    float t = std::min(t1, t2);

    return t;
}
