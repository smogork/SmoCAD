//
// Created by ksm on 4/7/22.
//

#include "Selectable.h"
#include "Scene/Systems/SelectableSystem.h"
#include "Scene/SceneECS.h"

std::shared_ptr<Selectable> Selectable::CreateRegisteredComponent(unsigned int oid)
{
    if (auto scene = SceneECS::Instance().lock())
    {
        auto system = scene->GetSystem<SelectableSystem>().lock();
        return system->CreateRegistered(oid);
    }
    return nullptr;
}

void Selectable::UnregisterComponent()
{
    if (auto scene = SceneECS::Instance().lock())
    {
        auto system = scene->GetSystem<SelectableSystem>().lock();
        system->Unregister(GetAttachedObjectID());
    }
}

Selectable::Selectable(unsigned int oid): IComponent(oid, SELECTABLE)
{
    Selected = false;
}

Selectable::~Selectable()
{
    UnregisterComponent();
}

