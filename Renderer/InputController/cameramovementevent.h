#ifndef CAMERAMOVEMENTEVENT_H
#define CAMERAMOVEMENTEVENT_H

#include <QEvent>
#include <QMatrix4x4>

class CameraMovementEvent: public QEvent
{
public:
    CameraMovementEvent(QMatrix4x4 viewMatrix);

    QMatrix4x4 NewViewMatrix;
};

#endif // CAMERAMOVEMENTEVENT_H
