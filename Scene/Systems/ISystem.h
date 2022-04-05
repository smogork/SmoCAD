//
// Created by ksm on 4/3/22.
//

#ifndef SMOCAD_ISYSTEM_H
#define SMOCAD_ISYSTEM_H

#include <map>
#include <memory>
#include <vector>
#include "IAbstractSystem.h"

enum SYSTEM_ID
{
    TRANSFORM,
    DRAWING,
    SYSTEM_COUNT
};

template <typename C>
class ISystem: public IAbstractSystem
{
private:
    unsigned int systemID;
    std::map<unsigned int, std::weak_ptr<C>> components;

public:
    ISystem() = delete;
    explicit ISystem(unsigned int sid) { systemID = sid; }

    unsigned int GetSystemID() { return systemID; }

    std::weak_ptr<C> GetComponent(unsigned int oid)
    {
        if (components.contains(oid))
            return components[oid];
        return std::shared_ptr<C>(nullptr);
    }

    std::vector<std::weak_ptr<C>> GetComponents()
    {
        std::vector<std::weak_ptr<C>> res;
        for (std::pair<unsigned int, std::weak_ptr<C>> p : components)
            res.push_back(p.second);
        return res;
    }

    std::shared_ptr<C> CreateRegistered(unsigned int oid)
    {
        std::shared_ptr<C> res = std::make_shared<C>(oid);
        if (components.insert(std::make_pair(oid, res)).second)
            return res;
        return nullptr;
    }

    bool RegisterComponent(std::shared_ptr<C> component)
    {
        return components.insert(std::make_pair(component->GetAttachedObjectID(), component)).second;
    }

    bool Unregister(unsigned int oid)
    {
        return components.erase(oid) > 0;
    }

};


#endif //SMOCAD_ISYSTEM_H
