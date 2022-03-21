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

    explicit SceneMouseClickEvent(QVector3D near, QVector3D far) : ClickViewPointFar(far), ClickViewPointNear(near) {}
};


#endif //SMOCAD_SCENEMOUSECLICKEVENT_H
