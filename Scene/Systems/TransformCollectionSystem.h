//
// Created by ksm on 4/9/22.
//

#ifndef SMOCAD_TRANSFORMCOLLECTIONSYSTEM_H
#define SMOCAD_TRANSFORMCOLLECTIONSYSTEM_H

#include "ISystem.h"
#include "Scene/Components/TransformCollection.h"

class TransformCollectionSystem: public ISystem<TransformCollection>
{
public:
    TransformCollectionSystem(): ISystem(SYSTEM_ID::TRANSFORM_COLLECTION)
    { }
    const char* GetSystemName() override { return "TransformCollectionSystem"; }
};


#endif //SMOCAD_TRANSFORMCOLLECTIONSYSTEM_H
