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
    static bool DoesSegmentsCross(QPoint A1, QPoint B1, QPoint A2, QPoint B2);
    static bool DoesSegmentsCross(QVector2D A1, QVector2D B1, QVector2D A2, QVector2D B2);
    static std::pair<QVector2D, QVector2D> GetSegmentsCrossPoint(QPoint A1, QPoint B1, QPoint A2, QPoint B2);
    static std::pair<QVector2D, QVector2D> GetSegmentsCrossPoint(QVector2D A1, QVector2D B1, QVector2D A2, QVector2D B2, bool ignoreEndCross = false);
    static bool DoesSegmentCrossRect(QPoint A, QPoint B, QRect rect);
    static bool DoesSegmentCrossRect(QVector2D A, QVector2D B, QVector4D rect);
    static bool GetDirectionOnSegmentsTurnLeft(QVector2D A1, QVector2D B1, QVector2D A2, QVector2D B2);
    static bool GetDirectionOnSegmentsTurnRight(QVector2D A1, QVector2D B1, QVector2D A2, QVector2D B2);
};

#endif //SMOCAD_GEOMETRYRELATION_H
