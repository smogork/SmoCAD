//
// Created by ksm on 3/12/22.
//

#include "TransformableObject.h"
#include <Qt3DCore>
#include <QTransform>

TransformableObject::TransformableObject(QVector3D position): TransformableObject(position, QVector3D(), QVector3D(1, 1, 1)) {}


TransformableObject::TransformableObject(QVector3D position, QVector3D rotation, QVector3D scale)
{
    this->Position = position;
    this->Rotation = rotation;
    this->Scale = scale;
}

QMatrix4x4 TransformableObject::GetModelMatrix()
{
    QMatrix4x4 result;
    result.setToIdentity();
    result.translate(Position);
    result.rotate(Rotation.x(), GetXAxis());
    result.rotate(Rotation.y(), GetYAxis());
    result.rotate(Rotation.z(), GetZAxis());
    result.scale(Scale);
    return result;
}

TransformableObject TransformableObject::DecomposeTransformations(QMatrix4x4 transform)
{
    Qt3DCore::QTransform test;
    test.setMatrix(transform);
    return TransformableObject(test.translation(), test.rotation().toEulerAngles(), test.scale3D());
}
