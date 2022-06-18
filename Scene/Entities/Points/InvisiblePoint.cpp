//
// Created by ksm on 4/22/22.
//

#include "InvisiblePoint.h"

InvisiblePoint::InvisiblePoint(QVector3D pos): IEntity(INVISIBLE_POINT_CLASS)
{
    AddComponent(p_Transform = Transform::CreateRegisteredComponent(GetObjectID(), pos));
    AddComponent(p_CollectionAware = CollectionAware::CreateRegisteredComponent(GetObjectID(), p_Transform));
}
