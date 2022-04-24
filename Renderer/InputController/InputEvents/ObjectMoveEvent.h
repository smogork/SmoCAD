//
// Created by ksm on 4/23/22.
//

#ifndef SMOCAD_OBJECTMOVEEVENT_H
#define SMOCAD_OBJECTMOVEEVENT_H

#include <QEvent>
#include <QPoint>
#include <QVector3D>

class ObjectMoveEvent: public QEvent
{
public:
    QVector4D RaycastDirection;
    QVector4D RaycastStart;
    QVector3D CameraBackVersor;

    explicit ObjectMoveEvent(QPoint screenPoint);
};

#endif //SMOCAD_OBJECTMOVEEVENT_H
