//
// Created by ksm on 4/13/22.
//

#ifndef SMOCAD_SCREENSELECTABLESYSTEM_H
#define SMOCAD_SCREENSELECTABLESYSTEM_H

#include "ISystem.h"
#include "Scene/Components/ScreenSelectable.h"
#include "Renderer/InputController/InputEvents/SceneMouseClickEvent.h"

class ScreenSelectableSystem: public ISystem<ScreenSelectable>
{
public:
    ScreenSelectableSystem(): ISystem(SYSTEM_ID::SELECTABLE)
    { }

    const char* GetSystemName() override { return "ScreenSelectableSystem"; }

    std::shared_ptr<ScreenSelectable> SelectObject(std::shared_ptr<SceneMouseClickEvent> event);
};


#endif //SMOCAD_SCREENSELECTABLESYSTEM_H
