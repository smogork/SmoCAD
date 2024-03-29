//
// Created by ksm on 4/3/22.
//

#include "Transform.h"
#include "Scene/SceneECS.h"
#include "Scene/Systems/TransformSystem.h"
#include <Qt3DCore>

Transform::Transform(unsigned int oid):
Transform(oid, QVector3D(), QVector3D(), QVector3D(1, 1, 1)) {}


Transform::Transform(unsigned int oid, QVector3D position, QVector3D rotation, QVector3D scale): IComponent(oid, SYSTEM_ID::TRANSFORM)
{
    this->Position = position;
    this->Rotation = rotation;
    this->Scale = scale;
}

QMatrix4x4 Transform::GetModelMatrix()
{
    QMatrix4x4 result;
    result.setToIdentity();
    result.translate(Position);
    result.rotate(QQuaternion::fromEulerAngles(Rotation));
    result.scale(Scale);
    return result;
}

void Transform::DecomposeTransformations(QMatrix4x4 transform)
{
    if (Locked)
        return;

    Qt3DCore::QTransform test;
    test.setMatrix(transform);
    Position = test.translation();
    Rotation = test.rotation().toEulerAngles();
    Scale = test.scale3D();
}

void Transform::UnregisterComponent()
{
    if (auto scene = SceneECS::Instance().lock())
    {
        auto system = scene->GetSystem<TransformSystem>().lock();
        system->Unregister(GetAttachedObjectID());
    }
}

Transform::~Transform()
{
    UnregisterComponent();
}

std::shared_ptr<Transform>  Transform::CreateRegisteredComponent(unsigned int oid, QVector3D position)
{
    if (auto scene = SceneECS::Instance().lock())
    {
        if (auto system = scene->GetSystem<TransformSystem>().lock())
        {
           auto item = system->CreateRegistered(oid);
           item->Position = position;
           return item;
        }
    }
    return nullptr;
}

void Transform::SetPosition(QVector3D pos)
{
    if (Locked)
        return;
    Position = pos;
}


