//
// Created by ksm on 3/27/22.
//

#ifndef SMOCAD_BEZIERCURVEC0_H
#define SMOCAD_BEZIERCURVEC0_H


#include "IRenderableObject.h"
#include "PointObject.h"

class BezierCurveC0: IRenderableObject
{
private:
    std::list<QVector3D> controlPoints;

public:
    
};


#endif //SMOCAD_BEZIERCURVEC0_H
