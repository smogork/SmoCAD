//
// Created by ksm on 3/12/22.
//

#include "TorusObject.h"

#include <QMatrix3x3>

void TorusObject::SetBiggerRadius(float value)
{
    biggerR = value > 0.1f ? value : 0.1f;
}

void TorusObject::SetSmallerRadius(float value)
{
    smallerR = value > 0.1f ? value : 0.1f;
}

TorusObject::TorusObject(QVector3D pos, float R, float r, int RDensity, int rDensity)
        : TransformableObject(pos)
{
    SetBiggerRadius(R);
    SetSmallerRadius(r);
    SetBiggerRadiusDensity(RDensity);
    SetSmallerRadiusDensity(rDensity);
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
