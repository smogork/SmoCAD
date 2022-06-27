//
// Created by ksm on 4/10/22.
//

#include "Utilites.h"

QVector4D ColorToVector4D(QColor c)
{
    return {c.redF(), c.greenF(), c.blueF(), c.alphaF()};
}

float wrap(float v, float min, float max)
{
    v = v - min;
    float m = max - min;
    int i = floor(v / m);
    return fmodf(v - i * m , m) + min;
}

int wrap(int v, int min, int max)
{
    v = v - min;
    int m = max - min;
    int i = v / m;
    return ((v - i * m) % m) + min;
}

MG1::Float3 SerializeQVector3D(QVector3D v)
{
    MG1::Float3 res;
    res.x = v.x();
    res.y = v.y();
    res.z = v.z();

    return res;
}

QVector3D DeserializeFloat3(MG1::Float3 v)
{
    return {v.x, v.y, v.z};
}