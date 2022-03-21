#ifndef CAMERAMOVEMENTEVENT_H
#define CAMERAMOVEMENTEVENT_H

#include <QEvent>
#include <QMatrix4x4>

class CameraUpdateEvent: public QEvent
{
public:
    CameraUpdateEvent(QMatrix4x4 viewMatrix);

    QMatrix4x4 NewViewMatrix;
};

#endif // CAMERAMOVEMENTEVENT_H
