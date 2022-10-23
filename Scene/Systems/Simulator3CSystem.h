//
// Created by ksm on 10/23/22.
//

#ifndef SMOCAD_SIMULATOR3CSYSTEM_H
#define SMOCAD_SIMULATOR3CSYSTEM_H

#include "Scene/Components/Simulator3CComponent.h"
#include "ISystem.h"

class Simulator3CSystem : public ISystem<Simulator3CComponent>
{
public:
    Simulator3CSystem() : ISystem(SYSTEM_ID::SIMULATOR3C) {}
    
    const char *GetSystemName() override { return "Simulator3CSystem"; }
    std::shared_ptr<Simulator3CComponent>
    CreateRegistered(unsigned int oid, std::shared_ptr<Transform> simulatorTransform);
};

#endif //SMOCAD_SIMULATOR3CSYSTEM_H
