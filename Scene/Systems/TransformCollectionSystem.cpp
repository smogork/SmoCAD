//
// Created by ksm on 4/9/22.
//

#include "TransformCollectionSystem.h"
#include "Controls/transformcollectioncontrol.h"

std::unique_ptr<ComponentControl> TransformCollectionSystem::PrepareUIForObject(unsigned int oid)
{
    if (auto collection = GetComponent(oid).lock())
        return std::move(std::make_unique<TransformCollectionControl>(collection));
    return nullptr;
}
