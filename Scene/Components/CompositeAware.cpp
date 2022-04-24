//
// Created by ksm on 4/9/22.
//

#include "CompositeAware.h"
#include "Scene/SceneECS.h"
#include "Scene/Systems/CompositeAwareSystem.h"

const QColor CompositeAware::CompositeColor = Qt::cyan;

std::shared_ptr<CompositeAware>
CompositeAware::CreateRegisteredComponent(unsigned int oid, std::shared_ptr<Transform> t, std::shared_ptr<Drawing> d)
{
    if (auto scene = SceneECS::Instance().lock())
    {
        if (auto system = scene->GetSystem<CompositeAwareSystem>().lock())
        {
            auto item = system->CreateRegistered(oid);
            item->p_Drawing = d;
            item->p_Transform = t;
            return item;
        }
    }
    return nullptr;
}

void CompositeAware::UnregisterComponent()
{
    if (auto scene = SceneECS::Instance().lock())
    {
        auto system = scene->GetSystem<CompositeAwareSystem>().lock();
        system->Unregister(GetAttachedObjectID());
    }
}

CompositeAware::CompositeAware(unsigned int oid): IComponent(oid, COMPOSITE_AWARE)
{

}

CompositeAware::~CompositeAware()
{
    UnregisterComponent();
}
