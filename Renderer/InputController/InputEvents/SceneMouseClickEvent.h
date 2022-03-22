//
// Created by ksm on 3/21/22.
//

#ifndef SMOCAD_SCENEMOUSECLICKEVENT_H
#define SMOCAD_SCENEMOUSECLICKEVENT_H


#include <QVector3D>

class SceneMouseClickEvent
{
public:
    QVector3D ClickViewPointNear;
    QVector3D ClickViewPointFar;
    QVector4D RaycastDirection;
    QVector4D RaycastStart;

    explicit SceneMouseClickEvent(QVector3D near, QVector3D far) : ClickViewPointFar(far), ClickViewPointNear(near)
    {
        RaycastDirection = (ClickViewPointFar - ClickViewPointNear).toVector4D();
        RaycastStart = ClickViewPointNear.toVector4D();
        RaycastStart.setW(1.0f);
    }
};


#endif //SMOCAD_SCENEMOUSECLICKEVENT_H
