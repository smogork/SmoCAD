//
// Created by ksm on 4/7/22.
//

#include <cfloat>
#include "SelectableSystem.h"

void SelectableSystem::ClearSystem()
{
    selectedObject = nullptr;

}

std::shared_ptr<Selectable> SelectableSystem::CreateRegistered(unsigned int oid)
{
    auto item = ISystem::CreateRegistered(oid);

    auto notifier = item->Selected.addNotifier([this,item](){
        if (item->Selected)
        {
            if (this->selectedObject)
                this->selectedObject->Selected = false;
            selectedObject = item;
        }
    });
    notifiers.insert(std::make_pair(item->GetAttachedObjectID(), std::move(notifier)));

    return item;
}

bool SelectableSystem::Unregister(unsigned int oid)
{
    notifiers.erase(oid);
    return ISystem::Unregister(oid);
}

bool SelectableSystem::RegisterComponent(std::shared_ptr<Selectable> component)
{
    bool res = ISystem::RegisterComponent(component);
    if (res)
    {
        auto notifier = component->Selected.addNotifier([this,component](){
            if (component->Selected)
            {
                if (this->selectedObject)
                    this->selectedObject->Selected = false;
                selectedObject = component;
            }
        });
        notifiers.insert(std::make_pair(component->GetAttachedObjectID(), std::move(notifier)));
    }
    return res;
}
