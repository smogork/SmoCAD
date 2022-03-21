#include "CameraUpdateEvent.h"


CameraUpdateEvent::CameraUpdateEvent(QMatrix4x4 viewMatrix)
    :QEvent(QEvent::User), NewViewMatrix(viewMatrix)
{ }
