//
// Created by ksm on 3/12/22.
//

#ifndef SMOCAD_TORUSOBJECT_H
#define SMOCAD_TORUSOBJECT_H


#include "TransformableObject.h"
#include "IRenderableObject.h"

class TorusObject: TransformableObject, IRenderableObject
{
private:
    float biggerR;
    float smallerR;
    int biggerRDensity;
    int smallerRDensity;
public:

};


#endif //SMOCAD_TORUSOBJECT_H
