//
// Created by ksm on 6/11/22.
//

#ifndef SMOCAD_GEOMETRYRELATION_H
#define SMOCAD_GEOMETRYRELATION_H

#include <QRect>
#include <QVector2D>

class GeometryRelation
{
public:
    static bool IsPointInsideRect(QPoint p, QRect rect);
    static bool IsSegmentInsideRect(QPoint A, QPoint B, QRect rect);
    static bool IsSegmentInsideRect(QVector2D A, QVector2D B, QVector4D rect);
};

#endif //SMOCAD_GEOMETRYRELATION_H
