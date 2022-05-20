//
// Created by ksm on 5/17/22.
//

#ifndef SMOCAD_UVPLANECREATORSYSTEM_H
#define SMOCAD_UVPLANECREATORSYSTEM_H

#include "ISystem.h"
#include "Scene/Components/UvPlainCreator.h"

class UVPlaneCreatorSystem: public ISystem<UVPlaneCreator>
{
public:
    UVPlaneCreatorSystem(): ISystem(UV_PLANE_CREATOR) { }

    const char* GetSystemName() override { return "UVPlaneCreatorSystem"; }
    std::unique_ptr<ComponentControl> PrepareUIForObject(unsigned int oid) override;
};

#endif //SMOCAD_UVPLANECREATORSYSTEM_H
