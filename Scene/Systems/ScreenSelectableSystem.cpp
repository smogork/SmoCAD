//
// Created by ksm on 4/13/22.
//

#include <cfloat>
#include "ScreenSelectableSystem.h"

std::shared_ptr<ScreenSelectable> ScreenSelectableSystem::SelectObject(std::shared_ptr<SceneMouseClickEvent> event)
{
    float t_min = FLT_MAX;
    std::shared_ptr<ScreenSelectable> closest = nullptr;
    for (const std::pair<unsigned int, std::weak_ptr<ScreenSelectable>> &s : components)
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
        closest->p_Selected->Selected = true;
        return closest;
    }

    return nullptr;
}