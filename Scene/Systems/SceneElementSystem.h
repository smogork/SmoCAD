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
    Q_OBJECT
private:
    std::map<unsigned int, std::unique_ptr<QListWidgetSceneElement>> listItems;
    QListWidget *sceneElementList;
    
    void onRemoveSceneElement();
    void onRenameSceneElement();
    void CreateBezierC0();
    void CreateBezierC2();
    void CreateInterpolationC2();

public:
    SceneElementSystem() : ISystem(SYSTEM_ID::SCENE_ELEMENT), sceneElementList(nullptr) {}
    
    const char *GetSystemName() override { return "SceneElementSystem"; }
    
    std::shared_ptr<SceneElement>
    CreateRegistered(unsigned int oid, std::shared_ptr<Selectable> select, std::shared_ptr<CompositeAware> composite);
    bool RegisterComponent(std::shared_ptr<SceneElement> component) override;
    bool Unregister(unsigned int oid) override;
    
    void AttachItemList(QListWidget *list);
    std::unique_ptr<QMenu> CreateContextMenuForSelection();
    
    //Wpisz do globalnego obiektu serializacyjnego wszystkie SceneElements
    void SerializeSceneObjects();

signals:
    void RequestRepaint();
    void RequestControlsUpdate(unsigned int oid);
};

#endif //SMOCAD_SCENEELEMENTSYSTEM_H
