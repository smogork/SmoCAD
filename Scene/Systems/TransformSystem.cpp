//
// Created by ksm on 4/3/22.
//

#include "TransformSystem.h"
#include "Controls/transformcontrol.h"

/*std::weak_ptr<Transform> TransformSystem::GetComponent(unsigned int oid)
{
    if (components.contains(oid))
        return components[oid];
    return std::shared_ptr<Transform>(nullptr);
}

std::vector<std::weak_ptr<Transform>> TransformSystem::GetComponents()
{
    std::vector<std::weak_ptr<Transform>> res;
    for (std::pair<unsigned int, std::weak_ptr<Transform>> p : components)
        res.push_back(p.second);
    return res;
}

std::shared_ptr<Transform> TransformSystem::CreateRegistered(unsigned int oid)
{
    std::shared_ptr<Transform> res = std::make_shared<Transform>(oid);
    if (components.insert(std::make_pair(oid, res)).second)
        return res;
    return nullptr;
}

bool TransformSystem::Unregister(unsigned int oid)
{
    return components.erase(oid) > 0;
}*/
std::unique_ptr<ComponentControl> TransformSystem::PrepareUIForObject(unsigned int oid)
{
    if (auto transform = GetComponent(oid).lock())
        return std::move(std::make_unique<TransformControl>(transform));
    return nullptr;
}
