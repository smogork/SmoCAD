//
// Created by ksm on 4/13/22.
//

#ifndef SMOCAD_SCENEELEMENTSYSTEM_H
#define SMOCAD_SCENEELEMENTSYSTEM_H


#include <QListWidgetItem>
#include "ISystem.h"
#include "Scene/Components/SceneElement.h"
#include "Controls/ListElements/QListWidgetSceneElement.h"

class SceneElementSystem: public ISystem<SceneElement>
{
private:
    std::map<unsigned int, std::unique_ptr<QListWidgetSceneElement>> listItems;
    QListWidget* sceneElementList;

public:
    SceneElementSystem(): ISystem(SYSTEM_ID::SCENE_ELEMENT), sceneElementList(nullptr) { }

    const char* GetSystemName() override { return "SceneElementSystem"; }
    std::shared_ptr<SceneElement> CreateRegistered(unsigned int oid) override;
    bool RegisterComponent(std::shared_ptr<SceneElement> component) override;
    bool Unregister(unsigned int oid) override;

    void AttachItemList(QListWidget* list);
};


#endif //SMOCAD_SCENEELEMENTSYSTEM_H
