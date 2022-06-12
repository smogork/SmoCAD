//
// Created by ksm on 6/11/22.
//

#ifndef SMOCAD_GEOMETRYRELATION_H
#define SMOCAD_GEOMETRYRELATION_H

#include <QRect>

class GeometryRelation
{
public:
    static bool IsPointInsideRect(QPoint p, QRect rect);
};

#endif //SMOCAD_GEOMETRYRELATION_H
