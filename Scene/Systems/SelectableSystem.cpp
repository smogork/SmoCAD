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

    std::weak_ptr<Selectable> weakItem = item;
    auto notifier = item->Selected.addNotifier([this,weakItem](){
        if (auto s = weakItem.lock())
        {
            if (s->Selected)
            {
                if (this->selectedObject)
                    this->selectedObject->Selected = false;
                selectedObject = s;
            }
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
        std::weak_ptr<Selectable> weakItem = component;
        auto notifier = component->Selected.addNotifier([this,weakItem](){
            if (auto s = weakItem.lock())
            {
                if (s->Selected)
                {
                    if (this->selectedObject)
                        this->selectedObject->Selected = false;
                    selectedObject = s;
                }
            }
        });
    }
    return res;
}

void SelectableSystem::Unselect()
{
    if (selectedObject)
    {
        this->selectedObject->Selected = false;
        this->selectedObject.reset();
    }
}
