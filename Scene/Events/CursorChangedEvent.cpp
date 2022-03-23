//
// Created by ksm on 3/22/22.
//

#include "CursorChangedEvent.h"

CursorChangedEvent::CursorChangedEvent(QVector3D wPos, QPoint vPos)
: QEvent(QEvent::User), WorldPosition(wPos), ViewPosition(vPos)
{

}
