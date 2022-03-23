//
// Created by ksm on 3/22/22.
//

#ifndef SMOCAD_SELECTEDOBJECTCHANGEDEVENT_H
#define SMOCAD_SELECTEDOBJECTCHANGEDEVENT_H

#include <QEvent>

#include "Objects/IRenderableObject.h"

class SelectedObjectChangedEvent : public QEvent
{
public:
    //[TODO] Poprzerabiac numery eventów zdefiniowanych w projekcie
    SelectedObjectChangedEvent(TransformableObject *obj) : ObjectToTransform(obj), QEvent(QEvent::User) {}

    TransformableObject *ObjectToTransform;
};


#endif //SMOCAD_SELECTEDOBJECTCHANGEDEVENT_H
