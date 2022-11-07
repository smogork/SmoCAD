//
// Created by ksm on 10/20/22.
//

#include "Simulator3C.h"

Simulator3C::Simulator3C(const QString &name) : Simulator3C(name, {0, 0, 0})
{ }

Simulator3C::Simulator3C(const QString &name, QVector3D position) : IEntity(SIMULATOR3C_CLASS)
{
    AddComponent(p_Transform = Transform::CreateRegisteredComponent(GetObjectID(), position));
    AddComponent(p_Selected = Selectable::CreateRegisteredComponent(GetObjectID()));
    AddComponent(p_SceneElement = SceneElement::CreateRegisteredComponent(GetObjectID(), name, p_Selected));
    AddComponent(p_Simulator = Simulator3CComponent::CreateRegisteredComponent(GetObjectID(), p_Transform));
}
