//
// Created by ksm on 10/20/22.
//

#include "Simulator3C.h"

Simulator3C::Simulator3C(const QString &name) : Simulator3C(name, {0, 0, 0})
{ }

Simulator3C::Simulator3C(const QString &name, QVector3D position) : IEntity(SIMULATOR3C_CLASS)
{
    AddComponent(p_Transform = Transform::CreateRegisteredComponent(GetObjectID(), position));
    AddComponent(p_Drawing = StaticDrawing::CreateRegisteredComponent(GetObjectID()));
}
