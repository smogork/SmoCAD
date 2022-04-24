//
// Created by ksm on 4/7/22.
//

#ifndef SMOCAD_SELECTABLESYSTEM_H
#define SMOCAD_SELECTABLESYSTEM_H

#include "ISystem.h"
#include "Scene/Components/Selectable.h"
#include "Renderer/InputController/InputEvents/SceneMouseClickEvent.h"
#include "Renderer/InputController/InputEvents/ObjectMoveEvent.h"
#include "Scene/Entities/Composite.h"

class SelectableSystem: public ISystem<Selectable>
{
    Q_OBJECT
private:
    std::shared_ptr<Selectable> selectedObject = nullptr;
    std::map<unsigned int, QPropertyNotifier> notifiers;

protected slots:
    void OnSelectedMoveRequest(std::shared_ptr<ObjectMoveEvent> event);

public:
    SelectableSystem();

    const char* GetSystemName() override { return "SelectableSystem"; }
    const std::shared_ptr<Selectable>& GetSelectedObject() {return selectedObject;}

    std::shared_ptr<Selectable> CreateRegistered(unsigned int oid) override;
    bool RegisterComponent(std::shared_ptr<Selectable> component) override;
    bool Unregister(unsigned int oid) override;
    void ClearSystem() override;

    void Unselect();
    //void AssignComposite(std::unique_ptr<Composite> composite);
};


#endif //SMOCAD_SELECTABLESYSTEM_H
