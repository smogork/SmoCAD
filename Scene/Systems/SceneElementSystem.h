//
// Created by ksm on 4/13/22.
//

#ifndef SMOCAD_SCENEELEMENTSYSTEM_H
#define SMOCAD_SCENEELEMENTSYSTEM_H


#include <QListWidgetItem>
#include "ISystem.h"
#include "Scene/Components/SceneElement.h"

class SceneElementSystem: public ISystem<SceneElement>
{
public:
    class QListWidgetSceneElement: public QListWidgetItem
    {
    private:
        std::weak_ptr<SceneElement> element;
        QPropertyNotifier nameNotifier;

    public:
        QListWidgetSceneElement(QListWidget* parent, std::shared_ptr<SceneElement> element);

        void SelectItem();
        unsigned int GetAttachedObjectID();
    };
private:
    std::map<unsigned int, std::unique_ptr<QListWidgetSceneElement>> listItems;
    QListWidget* sceneElementList;

public:
    SceneElementSystem(QListWidget* list): ISystem(SYSTEM_ID::SCENE_ELEMENT), sceneElementList(list)
    { }

    const char* GetSystemName() override { return "SceneElementSystem"; }
    std::shared_ptr<SceneElement> CreateRegistered(unsigned int oid) override;
    bool RegisterComponent(std::shared_ptr<SceneElement> component) override;
    bool Unregister(unsigned int oid) override;

    void AttachItemList(QListWidget* list);
};


#endif //SMOCAD_SCENEELEMENTSYSTEM_H
