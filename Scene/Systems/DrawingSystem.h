//
// Created by ksm on 4/3/22.
//

#ifndef SMOCAD_DRAWINGSYSTEM_H
#define SMOCAD_DRAWINGSYSTEM_H


#include "ISystem.h"
#include "Scene/Components/Drawing.h"

class DrawingSystem: public ISystem<Drawing>
{
public:
    DrawingSystem(): ISystem(SYSTEM_ID::DRAWING)
    { }

};


#endif //SMOCAD_DRAWINGSYSTEM_H
