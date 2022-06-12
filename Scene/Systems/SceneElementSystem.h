//
// Created by ksm on 4/13/22.
//

#ifndef SMOCAD_SCENEELEMENTSYSTEM_H
#define SMOCAD_SCENEELEMENTSYSTEM_H

#include <QListWidgetItem>
#include <QMenu>

#include "ISystem.h"
#include "Scene/Components/SceneElement.h"
#include "Controls/ListElements/QListWidgetSceneElement.h"

class SceneElementSystem : public ISystem<SceneElement>
{
private:
    std::map<unsigned int, std::unique_ptr<QListWidgetSceneElement>> listItems;
    QListWidget *sceneElementList;
    
    void RemoveSceneElement(const std::vector<unsigned int>& items);//przynajmniej jeden element
    void RenameSceneElement(std::shared_ptr<SceneElement> toRename);//tylko jeden element

public:
    SceneElementSystem() : ISystem(SYSTEM_ID::SCENE_ELEMENT), sceneElementList(nullptr) {}
    
    const char *GetSystemName() override { return "SceneElementSystem"; }
    
    std::shared_ptr<SceneElement>
    CreateRegistered(unsigned int oid, std::shared_ptr<Selectable> select, std::shared_ptr<CompositeAware> composite);
    bool RegisterComponent(std::shared_ptr<SceneElement> component) override;
    bool Unregister(unsigned int oid) override;
    
    std::list<std::pair<QString, std::function<void(const std::vector<unsigned int> &selectedOids)> > >
    CreateContextMenuItemsForScene(const std::vector<unsigned int> &selectedOids) override;
    std::list<std::pair<QString, std::function<void(const std::vector<unsigned int> &selectedOids,
                                                    const std::vector<unsigned int> &listContextOids)> > >
    CreateContextMenuItemsForSceneList(const std::vector<unsigned int> &selectedOids,
                                       const std::vector<unsigned int> &listContextOids) override;
    
    void AttachItemList(QListWidget *list);
    std::vector<unsigned int> GetSelectedItemsOnList();
    
    //Wpisz do globalnego obiektu serializacyjnego wszystkie SceneElements
    void SerializeSceneObjects();
    
};

#endif //SMOCAD_SCENEELEMENTSYSTEM_H
