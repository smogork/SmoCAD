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
private:
    std::shared_ptr<Selectable> selectedObject = nullptr;
    std::map<unsigned int, QPropertyNotifier> notifiers;

public:
    SelectableSystem(): ISystem(SYSTEM_ID::SELECTABLE)
    { }

    const char* GetSystemName() override { return "SelectableSystem"; }

    std::shared_ptr<Selectable> CreateRegistered(unsigned int oid) override;
    bool RegisterComponent(std::shared_ptr<Selectable> component) override;
    bool Unregister(unsigned int oid) override;
    void ClearSystem() override;

    void Unselect();
};


#endif //SMOCAD_SELECTABLESYSTEM_H
