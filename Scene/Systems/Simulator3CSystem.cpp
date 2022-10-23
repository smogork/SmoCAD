//
// Created by ksm on 10/23/22.
//

#include "Simulator3CSystem.h"

std::shared_ptr<Simulator3CComponent>
Simulator3CSystem::CreateRegistered(unsigned int oid, std::shared_ptr<Transform> simulatorTransform)
{
    std::shared_ptr<Simulator3CComponent> res = std::make_shared<Simulator3CComponent>(oid, simulatorTransform);
    if (components.insert(std::make_pair(oid, res)).second)
        return res;
    throw std::runtime_error("Error while creating component - already exists");
}
