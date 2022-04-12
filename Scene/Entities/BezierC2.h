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
    //[TODO] dodać entity klasy VirtualBezierC0 - nie będzie elementem sceny
    BezierC0 m_bezier;
    Polyline m_deBoorPolyline;
    std::list<std::unique_ptr<VirtualPoint>> bezierPoints;

    void CalculateBezierPoints();

public:
    std::shared_ptr<TransformCollection> p_Collection;

    BezierC2();
};


#endif //SMOCAD_BEZIERC2_H
