//
// Created by ksm on 4/9/22.
//

#include "CollectionAware.h"
#include "Scene/SceneECS.h"
#include "Scene/Systems/CollectionAwareSystem.h"

std::shared_ptr<CollectionAware>
CollectionAware::CreateRegisteredComponent(unsigned int oid, std::shared_ptr<Transform> t)
{
    if (auto scene = SceneECS::Instance().lock())
    {
        auto system = scene->GetSystem<CollectionAwareSystem>().lock();
        auto item = system->CreateRegistered(oid);
        item->p_Transform = t;
        return item;
    }
    return nullptr;
}

void CollectionAware::UnregisterComponent()
{
    if (auto scene = SceneECS::Instance().lock())
    {
        auto system = scene->GetSystem<CollectionAwareSystem>().lock();
        system->Unregister(GetAttachedObjectID());
    }
}

CollectionAware::CollectionAware(unsigned int oid): IComponent(oid, COLLECTION_AWARE)
{

}

CollectionAware::~CollectionAware()
{
    UnregisterComponent();
}
