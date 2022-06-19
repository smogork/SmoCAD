//
// Created by ksm on 6/19/22.
//

#include "IntersectionAware.h"
#include "Scene/SceneECS.h"
#include "Scene/Systems/Awares/IntersectionAwareSystem.h"

std::shared_ptr<IntersectionAware>
IntersectionAware::CreateRegisteredComponent(unsigned int oid, std::shared_ptr<UVParams> uv)
{
    if (auto scene = SceneECS::Instance().lock())
    {
        if (auto system = scene->GetSystem<IntersectionAwareSystem>().lock())
        {
            auto item = system->CreateRegistered(oid);
            item->p_UV = uv;
            return item;
        }
    }
    return nullptr;
}

void IntersectionAware::UnregisterComponent()
{
    if (auto scene = SceneECS::Instance().lock())
    {
        auto system = scene->GetSystem<IntersectionAwareSystem>().lock();
        system->Unregister(GetAttachedObjectID());
    }
}

IntersectionAware::IntersectionAware(unsigned int oid): IComponent(oid, INTERSECTION_AWARE)
{

}

IntersectionAware::~IntersectionAware()
{
    UnregisterComponent();
}
