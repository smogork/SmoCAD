//
// Created by ksm on 3/21/22.
//

#ifndef SMOCAD_SCENEMOUSECLICKEVENT_H
#define SMOCAD_SCENEMOUSECLICKEVENT_H

#include <QVector3D>
#include <QEvent>

class SceneMouseClickEvent: public QEvent
{
public:
    QPoint ViewClickPoint;
    QVector3D ClickViewPointNear;
    QVector3D ClickCenterPlainPoint;
    QVector3D ClickViewPointFar;
    QVector4D RaycastDirection;
    QVector4D RaycastStart;
    float PivotLength;

    explicit SceneMouseClickEvent(QPoint vPos);
};


#endif //SMOCAD_SCENEMOUSECLICKEVENT_H
