//
// Created by ksm on 4/9/22.
//

#include "TransformCollection.h"
#include "Scene/SceneECS.h"
#include "Scene/Systems/TransformCollectionSystem.h"

std::shared_ptr<TransformCollection> TransformCollection::CreateRegisteredComponent(unsigned int oid)
{
    if (auto scene = SceneECS::Instance().lock())
    {
        auto system = scene->GetSystem<TransformCollectionSystem>().lock();
        return system->CreateRegistered(oid);
    }
    return nullptr;
}

void TransformCollection::UnregisterComponent()
{
    if (auto scene = SceneECS::Instance().lock())
    {
        auto system = scene->GetSystem<TransformCollectionSystem>().lock();
        system->Unregister(GetAttachedObjectID());
    }
}

TransformCollection::TransformCollection(unsigned int oid): IComponent(oid, TRANSFORM_COLLECTION)
{

}

TransformCollection::~TransformCollection()
{
    UnregisterComponent();
}

void TransformCollection::AddPoint(std::shared_ptr<CollectionAware> newObject)
{
    for (const std::shared_ptr<Transform>& el : Points)
        if (el->GetAttachedObjectID() == newObject->GetAttachedObjectID())
            return;

    Points.push_back(newObject->p_Transform);
}
