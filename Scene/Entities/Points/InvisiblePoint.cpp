//
// Created by ksm on 4/22/22.
//

#include "InvisiblePoint.h"

InvisiblePoint::InvisiblePoint(QVector3D pos): IEntity(INVISIBLE_POINT_CLASS)
{
    AddComponent(p_Transform = Transform::CreateRegisteredComponent(objectID, pos));
    AddComponent(p_CollectionAware = CollectionAware::CreateRegisteredComponent(objectID, p_Transform));
}
