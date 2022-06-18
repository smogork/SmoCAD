//
// Created by ksm on 5/17/22.
//

#include "UvPlaneCreatorSystem.h"
#include "Controls/uvplanecreatorcontrol.h"

std::unique_ptr<ComponentControl> UVPlaneCreatorSystem::PrepareUIForObject(unsigned int oid)
{
    if (auto uv = GetComponent(oid).lock())
        return std::move(std::make_unique<UVPlaneCreatorControl>(uv));
    return nullptr;
}
