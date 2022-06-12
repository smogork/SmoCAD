//
// Created by ksm on 6/11/22.
//

#include "SelectRectangleUpdateEvent.h"


SelectRectangleUpdateEvent::SelectRectangleUpdateEvent(QPoint startPoint, QPoint currentPoint, bool finish, bool select) :
    QEvent(QEvent::User), DeleteRectangle(finish), SelectItems(select)
{
    SelectedArea = QRect(startPoint, currentPoint).normalized();
}
