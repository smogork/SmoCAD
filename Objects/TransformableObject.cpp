//
// Created by ksm on 3/12/22.
//

#include "TransformableObject.h"
#include "Renderer/SceneRenderer.h"

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
    result.rotate(Rotation.x(), SceneRenderer::GetXAxis());
    result.rotate(Rotation.y(), SceneRenderer::GetYAxis());
    result.rotate(Rotation.z(), SceneRenderer::GetZAxis());
    result.scale(Scale);
    result.translate(Position);
    return result;
}
