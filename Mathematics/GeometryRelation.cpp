//
// Created by ksm on 6/11/22.
//

#include "GeometryRelation.h"

bool GeometryRelation::IsPointInsideRect(QPoint p, QRect rect)
{
    return rect.contains(p);
}

bool GeometryRelation::IsSegmentInsideRect(QPoint A, QPoint B, QRect rect)
{
    return false;
}

bool GeometryRelation::IsSegmentInsideRect(QVector2D A, QVector2D B, QVector4D rect)
{
    return false;
}
