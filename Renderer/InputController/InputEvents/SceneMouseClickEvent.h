//
// Created by ksm on 3/21/22.
//

#ifndef SMOCAD_SCENEMOUSECLICKEVENT_H
#define SMOCAD_SCENEMOUSECLICKEVENT_H


#include <QVector3D>

class SceneMouseClickEvent
{
public:
    QPoint ViewClickPoint;
    QVector3D ClickViewPointNear;
    QVector3D ClickViewPointFar;
    QVector4D RaycastDirection;
    QVector4D RaycastStart;

    explicit SceneMouseClickEvent(QPoint vPos, QVector3D near, QVector3D far) : ClickViewPointFar(far), ClickViewPointNear(near)
    {
        ViewClickPoint = vPos;
        RaycastDirection = (ClickViewPointFar - ClickViewPointNear).toVector4D();
        RaycastStart = ClickViewPointNear.toVector4D();
        RaycastStart.setW(1.0f);
    }
};


#endif //SMOCAD_SCENEMOUSECLICKEVENT_H
