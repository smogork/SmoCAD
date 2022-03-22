//
// Created by ksm on 3/12/22.
//

#include "TorusObject.h"

#include <QMatrix3x3>

TorusObject::TorusObject(QVector3D pos, std::shared_ptr<ShaderWrapper> shader, float R, float r, int RDensity, int rDensity)
        : IRenderableObject(pos, shader)
{
    SetBiggerRadius(R);
    SetSmallerRadius(r);
    SetBiggerRadiusDensity(RDensity);
    SetSmallerRadiusDensity(rDensity);

    vb = std::make_unique<QOpenGLBuffer>(QOpenGLBuffer::VertexBuffer);
    ib = std::make_unique<QOpenGLBuffer>(QOpenGLBuffer::IndexBuffer);
    CreateBuffers();
}

TorusObject::~TorusObject()
{
    vb->destroy();
    ib->destroy();
}

void TorusObject::SetBiggerRadius(float value)
{
    biggerR = value > 0.1f ? value : 0.1f;
}

void TorusObject::SetSmallerRadius(float value)
{
    smallerR = value > 0.1f ? value : 0.1f;
}

void TorusObject::SetBiggerRadiusDensity(int value)
{
    biggerRDensity = value < 4 ? 4 : value;
}

void TorusObject::SetSmallerRadiusDensity(int value)
{
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

    test = va->create();
    Shader->Bind();
    va->bind();
    test = vb->bind();

    //atrybuty shadera!!!
    //https://stackoverflow.com/questions/37999609/combining-vertex-array-object-with-vertex-buffer-index-buffer
    //ale to jest rozjebane
    int stride = 3 * sizeof(float); //only position on 3 floats
    //[TODO] Dodac klase opisujaca uklad buforow
    Shader->GetRawProgram()->enableAttributeArray(0);
    Shader->GetRawProgram()->setAttributeBuffer(0, GL_FLOAT, 0, 3, stride);

    test = ib->bind();
    va->release();

    vb->release();
    ib->release();
    Shader->Release();

    buffersCreated = true;
}


void TorusObject::Bind()
{
    Shader->SetUniform("u_MVP.Model", GetModelMatrix());
    IRenderableObject::Bind();
}

void TorusObject::UpdateBuffers()
{
    if (!buffersCreated)
        return;

    auto edges = GenerateTopologyEdges();
    auto vertices = GenerateGeometryVertices();

    vb->bind();
    vb->allocate(vertices.data(), sizeof(float) * vertices.size());
    vb->release();
    va->bind();
    ib->bind();
    ib->allocate(edges.data(), sizeof(int) * edges.size());
    va->release();
}
