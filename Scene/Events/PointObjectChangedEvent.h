//
// Created by ksm on 3/29/22.
//

#ifndef SMOCAD_POINTOBJECTCHANGEDEVENT_H
#define SMOCAD_POINTOBJECTCHANGEDEVENT_H


#include <QEvent>
#include "Objects/PointObject.h"

class PointObjectChangedEvent: QEvent
{
public:
    PointObject* ChangedPoint;
    bool IsRemoved;

    PointObjectChangedEvent(PointObject* p, bool removed): QEvent(QEvent::User), ChangedPoint(p), IsRemoved(removed) { }
};


#endif //SMOCAD_POINTOBJECTCHANGEDEVENT_H
