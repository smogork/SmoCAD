//
// Created by ksm on 6/11/22.
//

#include "SelectRectangleUpdateEvent.h"


SelectRectangleUpdateEvent::SelectRectangleUpdateEvent(QPoint startPoint, QPoint currentPoint, bool finish) :
    QEvent(QEvent::User), DeleteRectangle(finish)
{
    SelectedArea = QRect(startPoint, currentPoint).normalized();
}
