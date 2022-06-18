//
// Created by ksm on 4/10/22.
//

#include "Utilites.h"

QVector4D ColorToVector4D(QColor c)
{
    return {c.redF(), c.greenF(), c.blueF(), c.alphaF()};
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