#include "cameramovementevent.h"


CameraMovementEvent::CameraMovementEvent(QMatrix4x4 viewMatrix)
    :QEvent(QEvent::User), NewViewMatrix(viewMatrix)
{ }
