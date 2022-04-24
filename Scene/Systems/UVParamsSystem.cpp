//
// Created by ksm on 4/5/22.
//

#include "UVParamsSystem.h"
#include "Controls/uvcontrol.h"

std::unique_ptr<ComponentControl> UVParamsSystem::PrepareUIForObject(unsigned int oid)
{
    if (auto uv = GetComponent(oid).lock())
        return std::move(std::make_unique<UVControl>(uv));
    return nullptr;
}
