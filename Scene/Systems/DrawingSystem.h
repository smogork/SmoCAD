//
// Created by ksm on 4/3/22.
//

#ifndef SMOCAD_DRAWINGSYSTEM_H
#define SMOCAD_DRAWINGSYSTEM_H


#include "ISystem.h"
#include "Scene/Components/Drawing/Drawing.h"

class DrawingSystem: public ISystem<Drawing>
{
public:
    DrawingSystem(): ISystem(SYSTEM_ID::DRAWING)
    { }

    void Render(QOpenGLContext* context);
};


#endif //SMOCAD_DRAWINGSYSTEM_H
