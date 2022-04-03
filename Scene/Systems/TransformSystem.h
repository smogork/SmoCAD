//
// Created by ksm on 4/3/22.
//

#ifndef SMOCAD_TRANSFORMSYSTEM_H
#define SMOCAD_TRANSFORMSYSTEM_H

#include "Scene/Systems/ISystem.h"
#include "Scene/Components/Transform.h"

class TransformSystem: public ISystem
{
public:
    TransformSystem(): ISystem(SYSTEM_ID::TRANSFORM)
    { }

private:
    std::map<unsigned int, std::weak_ptr<Transform>> components;

public:

    std::weak_ptr<Transform> GetComponent(unsigned int oid);
    std::vector<std::weak_ptr<Transform>> GetComponents();
    std::shared_ptr<Transform> CreateRegistered(unsigned int oid);
    bool Unregister(unsigned int oid);

};


#endif //SMOCAD_TRANSFORMSYSTEM_H
