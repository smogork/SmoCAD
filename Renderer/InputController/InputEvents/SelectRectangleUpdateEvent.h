//
// Created by ksm on 6/11/22.
//

#ifndef SMOCAD_SELECTRECTANGLEUPDATEEVENT_H
#define SMOCAD_SELECTRECTANGLEUPDATEEVENT_H

#include <QEvent>
#include <QPoint>
#include <QRect>

class SelectRectangleUpdateEvent: public QEvent
{
public:
    SelectRectangleUpdateEvent(QPoint startPoint, QPoint currentPoint, bool finish = false, bool select = true);
    
    QRect SelectedArea;
    bool DeleteRectangle;
    bool SelectItems;
};

#endif //SMOCAD_SELECTRECTANGLEUPDATEEVENT_H
