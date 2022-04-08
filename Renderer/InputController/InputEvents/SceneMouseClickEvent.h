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
    QVector3D ClickCenterPlainPoint;
    QVector3D ClickViewPointFar;
    QVector4D RaycastDirection;
    QVector4D RaycastStart;
    float PivotLength;
    bool UnselectObjects;

    explicit SceneMouseClickEvent(QPoint vPos, QVector3D near, QVector3D far, bool unselect, float pivotLength, QVector4D centerPlain)
    : ClickViewPointFar(far), ClickViewPointNear(near), UnselectObjects(unselect), PivotLength(pivotLength)
    {
        ViewClickPoint = vPos;
        RaycastDirection = (ClickViewPointFar - ClickViewPointNear).toVector4D();
        RaycastStart = ClickViewPointNear.toVector4D();
        RaycastStart.setW(1.0f);
        
        float t = -QVector4D::dotProduct(centerPlain, RaycastStart) /
                  QVector4D::dotProduct(centerPlain, RaycastDirection);
        ClickCenterPlainPoint = (RaycastDirection * t + RaycastStart).toVector3D();

        /*qDebug() << "CenterViewPlain:" << plain;
        qDebug() << "ClickPoint:" << clickPoint;*/
    }
};


#endif //SMOCAD_SCENEMOUSECLICKEVENT_H
