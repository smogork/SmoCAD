//
// Created by ksm on 3/22/22.
//

#ifndef SMOCAD_CURSORCHANGEDEVENT_H
#define SMOCAD_CURSORCHANGEDEVENT_H


#include <QEvent>
#include <QVector3D>

class CursorChangedEvent: QEvent
{
public:
    QVector3D WorldPosition;
    QPoint ViewPosition;

    CursorChangedEvent(QVector3D wPos, QPoint vPos);
};


#endif //SMOCAD_CURSORCHANGEDEVENT_H
