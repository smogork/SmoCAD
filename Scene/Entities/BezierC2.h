//
// Created by ksm on 4/10/22.
//

#ifndef SMOCAD_BEZIERC2_H
#define SMOCAD_BEZIERC2_H


#include "IEntity.h"
#include "Scene/Components/Drawing/DynamicDrawing.h"
#include "Polyline.h"
#include "BezierC0.h"
#include "VirtualPoint.h"

class BezierC2: public IEntity
{
Q_OBJECT
private slots:
    void OnDeBoorModified();

private:
    BezierC0 m_bezier;
    std::list<std::unique_ptr<VirtualPoint>> bezierPoints;

    void CalculateBezierPoints();

public:
    Polyline DeBoorPolyline;

    BezierC2();
};


#endif //SMOCAD_BEZIERC2_H
