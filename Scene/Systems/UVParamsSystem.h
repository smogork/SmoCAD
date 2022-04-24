//
// Created by ksm on 4/5/22.
//

#ifndef SMOCAD_UVPARAMSSYSTEM_H
#define SMOCAD_UVPARAMSSYSTEM_H

#include "Scene/Systems/ISystem.h"
#include "Scene/Components/UVParams.h"

class UVParamsSystem: public ISystem<UVParams>
{
public:
    UVParamsSystem(): ISystem<UVParams>(UV) { }

    const char* GetSystemName() override { return "UVParamsSystem"; }
    std::unique_ptr<ComponentControl> PrepareUIForObject(unsigned int oid) override;
};


#endif //SMOCAD_UVPARAMSSYSTEM_H
