//
// Created by ksm on 3/12/22.
//

#include "TorusObject.h"

#include <QMatrix3x3>

TorusObject::TorusObject(QVector3D pos, float R, float r, int RDensity, int rDensity)
        : IRenderableObject(pos)
{
    SetBiggerRadius(R);
    SetSmallerRadius(r);
    SetBiggerRadiusDensity(RDensity);
    SetSmallerRadiusDensity(rDensity);

    vb = std::make_unique<QOpenGLBuffer>(QOpenGLBuffer::VertexBuffer);
    ib = std::make_unique<QOpenGLBuffer>(QOpenGLBuffer::IndexBuffer);
}

TorusObject::~TorusObject()
{
    vb->destroy();
    ib->destroy();
}

void TorusObject::SetBiggerRadius(float value)
{
    buffersToUpdate = true;
    biggerR = value > 0.1f ? value : 0.1f;
}

void TorusObject::SetSmallerRadius(float value)
{
    buffersToUpdate = true;
    smallerR = value > 0.1f ? value : 0.1f;
}

void TorusObject::SetBiggerRadiusDensity(int value)
{
    buffersToUpdate = true;
    biggerRDensity = value < 4 ? 4 : value;
}

void TorusObject::SetSmallerRadiusDensity(int value)
{
    buffersToUpdate = true;
    smallerRDensity = value < 4 ? 4 : value;
}

std::vector<float> TorusObject::GenerateGeometryVertices()
{
    std::vector<float> res(3 * smallerRDensity * biggerRDensity);

    int vIndex = 0;
    for (int u = 0; u < biggerRDensity; ++u)
    {
        float uDegree = u * 2.0f * M_PI / biggerRDensity;
        QMatrix4x4 rotY;
        rotY.rotate(uDegree * 180 / M_PI, GetYAxis());
        for (int v = 0; v < smallerRDensity; ++v)
        {
            float vDegree = v * 2.0f * M_PI / smallerRDensity;
            QVector4D p = rotY * QVector4D(
                    smallerR * cosf(vDegree) + biggerR,
                    smallerR * sinf(vDegree),
                    0.0f, 1.0f) ;

            res[vIndex] = p.x();
            res[vIndex + 1] = p.y();
            res[vIndex + 2] = p.z();
            vIndex += 3;
        }
    }

    return res;
}

std::vector<int> TorusObject::GenerateTopologyEdges()
{
    std::vector<int> res(GetIndexCount());

    int eIndex = 0;
    for (int u = 0; u < biggerRDensity; ++u)
    {
        int uOffset = u * smallerRDensity;

        //Mały okrąg
        for (int v = 0; v < smallerRDensity; ++v)
        {
            res[eIndex] = uOffset + v;
            res[eIndex + 1] = uOffset + ((v + 1) % smallerRDensity);
            eIndex += 2;
        }

        //Połaczenie z kolejnym okręgiem modulo
        for (int v = 0; v < smallerRDensity; ++v)
        {
            res[eIndex] = uOffset + v;
            res[eIndex + 1] = (uOffset + smallerRDensity + v) % (smallerRDensity * biggerRDensity);
            eIndex += 2;
        }
    }
    return res;
}

int TorusObject::GetIndexCount()
{
    return smallerRDensity * biggerRDensity * 4;
}

int TorusObject::GetBiggerRDensity()
{
    return biggerRDensity;
}

int TorusObject::GetSmallerRDensity()
{
    return smallerRDensity;
}

float TorusObject::GetSmallerR()
{
    return smallerR;
}

float TorusObject::GetBiggerR()
{
    return biggerR;
}

void TorusObject::CreateBuffers()
{
    bool test = vb->create();
    test = vb->bind();
    vb->setUsagePattern(QOpenGLBuffer::StaticDraw);
    auto vertices = GenerateGeometryVertices();
    vb->allocate(vertices.data(), sizeof(float) * vertices.size());
    vb->release();

    test = ib->create();
    test = ib->bind();
    ib->setUsagePattern(QOpenGLBuffer::StaticDraw);
    auto edges = GenerateTopologyEdges();
    ib->allocate(edges.data(), sizeof(int) * edges.size());
    ib->release();

    QOpenGLShaderProgram prog;
    prog.create();
    test = va->create();
    prog.bind();
    va->bind();
    test = vb->bind();

    //atrybuty shadera!!!
    //https://stackoverflow.com/questions/37999609/combining-vertex-array-object-with-vertex-buffer-index-buffer
    //ale to jest rozjebane
    int stride = 3 * sizeof(float); //only position on 3 floats
    //[TODO] Dodac klase opisujaca uklad buforow
    prog.enableAttributeArray(0);
    prog.setAttributeBuffer(0, GL_FLOAT, 0, 3, stride);

    test = ib->bind();
    va->release();

    vb->release();
    ib->release();
    prog.release();
}


void TorusObject::Bind(ShaderWrapper* shader)
{
    shader->SetUniform("u_MVP.Model", GetModelMatrix());
    if (Selected)
        shader->SetUniform("u_ObjectColor", QVector4D(0.8f, 0.8f, 0.8f, 1.0f));
    else
        shader->SetUniform("u_ObjectColor", QVector4D(1.0f, 0.5f, 0.2f, 1.0f));
    IRenderableObject::Bind(shader);
}

void TorusObject::UpdateBuffers()
{
    if (!AreBuffersCreated())
        return;

    auto edges = GenerateTopologyEdges();
    auto vertices = GenerateGeometryVertices();

    vb->bind();
    vb->allocate(vertices.data(), sizeof(float) * vertices.size());
    vb->release();
    //va->bind();
    ib->bind();
    ib->allocate(edges.data(), sizeof(int) * edges.size());
    //va->release();

    IRenderableObject::UpdateBuffers();
}

void TorusObject::DefineBuffers()
{
    CreateBuffers();
    IRenderableObject::DefineBuffers();
}
