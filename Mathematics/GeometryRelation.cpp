//
// Created by ksm on 6/11/22.
//

#include "GeometryRelation.h"

bool GeometryRelation::IsPointInsideRect(QPoint p, QRect rect)
{
    return rect.contains(p);
}
