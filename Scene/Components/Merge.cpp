//
// Created by ksm on 6/12/22.
//

#include "Merge.h"
#include "Scene/SceneECS.h"
#include "Scene/Systems/MergeSystem.h"

std::shared_ptr<Merge> Merge::CreateRegisteredComponent(unsigned int oid, std::shared_ptr<Transform> trans,
                                                        std::shared_ptr<CollectionAware> col)
{
    if (auto scene = SceneECS::Instance().lock())
    {
        auto system = scene->GetSystem<MergeSystem>().lock();
        auto item = system->CreateRegistered(oid);
        item->p_Transform = trans;
        item->p_CollectionAware = col;
        return item;
    }
    return nullptr;
}

void Merge::UnregisterComponent()
{
    if (auto scene = SceneECS::Instance().lock())
    {
        auto system = scene->GetSystem<MergeSystem>().lock();
        system->Unregister(GetAttachedObjectID());
    }
}

Merge::Merge(unsigned int oid): IComponent(oid, MERGE)
{

}

Merge::~Merge()
{
    UnregisterComponent();
}
