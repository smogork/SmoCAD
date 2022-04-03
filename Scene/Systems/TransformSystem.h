//
// Created by ksm on 4/3/22.
//

#ifndef SMOCAD_TRANSFORMSYSTEM_H
#define SMOCAD_TRANSFORMSYSTEM_H

#include "Scene/Systems/ISystem.h"
#include "Scene/Components/Transform.h"

class TransformSystem: public ISystem<Transform>
{
public:
    TransformSystem(): ISystem(SYSTEM_ID::TRANSFORM)
    { }

public:

};


#endif //SMOCAD_TRANSFORMSYSTEM_H
