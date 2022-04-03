//
// Created by ksm on 4/3/22.
//

#include "Point.h"

Point::Point(QVector3D pos): IEntity(POINT_CLASS)
{
    transform = Transform::CreateRegisteredComponent(objectID, pos);
    transform->Rotation.setX(90);
}

Point::~Point()
{
    transform->UnregisterComponent();
}
