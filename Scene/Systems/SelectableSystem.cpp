//
// Created by ksm on 4/7/22.
//

#include <cfloat>
#include "SelectableSystem.h"

std::shared_ptr<Selectable> SelectableSystem::SelectObject(std::shared_ptr<SceneMouseClickEvent> event)
{
    float t_min = FLT_MAX;
    std::shared_ptr<Selectable> closest = nullptr;
    for (const std::pair<unsigned int, std::weak_ptr<Selectable>> &s : components)
        if (auto obj = s.second.lock())
        {
            float t = obj->TestAgainstRaycast(event->RaycastStart, event->RaycastDirection, event->PivotLength);
            if (t != NAN && t < t_min)
            {
                t_min = t;
                closest = obj;
            }
        }

    if (closest)
    {
        if (selectedObject)
            selectedObject->SetSelection(false);
        selectedObject = closest;
        selectedObject->SetSelection(true);//?
        return selectedObject;
    }

    return nullptr;
}

void SelectableSystem::ClearSystem()
{
    selectedObject = nullptr;
}
