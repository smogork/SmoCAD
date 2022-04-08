//
// Created by ksm on 4/7/22.
//

#ifndef SMOCAD_SELECTABLESYSTEM_H
#define SMOCAD_SELECTABLESYSTEM_H

#include "ISystem.h"
#include "Scene/Components/Selectable.h"
#include "Renderer/InputController/InputEvents/SceneMouseClickEvent.h"

class SelectableSystem: public ISystem<Selectable>
{
    std::shared_ptr<Selectable> selectedObject = nullptr;

public:
    SelectableSystem(): ISystem(SYSTEM_ID::SELECTABLE)
    { }

    const char* GetSystemName() override { return "SelectableSystem"; }

    std::shared_ptr<Selectable> SelectObject(std::shared_ptr<SceneMouseClickEvent> event);
    void ClearSystem() override;
};


#endif //SMOCAD_SELECTABLESYSTEM_H
