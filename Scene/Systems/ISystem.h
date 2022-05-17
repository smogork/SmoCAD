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
    UV,
    SELECTABLE,
    COMPOSITE_AWARE,
    TRANSFORM_COLLECTION,
    COLLECTION_AWARE,
    SCREEN_SELECTABLE,
    SCENE_ELEMENT,
    UV_PLANE_CREATOR,
    SYSTEM_COUNT
};

template <typename C>
class ISystem: public IAbstractSystem
{
protected:
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

    bool IsObjectInSystem(unsigned int oid)
    {
        if (components.contains(oid))
            return true;
        return false;
    }

    std::vector<std::weak_ptr<C>> GetComponents()
    {
        std::vector<std::weak_ptr<C>> res;
        for (std::pair<unsigned int, std::weak_ptr<C>> p : components)
            res.push_back(p.second);
        return res;
    }

    virtual std::shared_ptr<C> CreateRegistered(unsigned int oid)
    {
        std::shared_ptr<C> res = std::make_shared<C>(oid);
        if (components.insert(std::make_pair(oid, res)).second)
            return res;
        return nullptr;
    }

    virtual bool RegisterComponent(std::shared_ptr<C> component)
    {
        return components.insert(std::make_pair(component->GetAttachedObjectID(), component)).second;
    }

    virtual bool Unregister(unsigned int oid)
    {
        return components.erase(oid) > 0;
    }

    int GetComponentCount() override
    {
        return components.size();
    }
};


#endif //SMOCAD_ISYSTEM_H
