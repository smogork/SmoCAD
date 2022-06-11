//
// Created by ksm on 4/13/22.
//

#include "SceneElementSystem.h"
#include "Scene/SceneECS.h"
#include "Scene/Scene.h"

#pragma region QListWidgetSceneElement

#pragma endregion

std::shared_ptr<SceneElement> SceneElementSystem::CreateRegistered(unsigned int oid, std::shared_ptr<Selectable> select,
                                                                   std::shared_ptr<CompositeAware> composite)
{
    auto item = ISystem::CreateRegistered(oid);
    item->p_Selected = select;
    item->p_CompositeAware = composite;
    
    std::unique_ptr<QListWidgetSceneElement> listItem =
            std::make_unique<QListWidgetSceneElement>(sceneElementList, item);
    listItems.insert(std::make_pair(item->GetAttachedObjectID(), std::move(listItem)));
    return item;
}

bool SceneElementSystem::RegisterComponent(std::shared_ptr<SceneElement> component)
{
    bool res = ISystem::RegisterComponent(component);
    if (res)
    {
        std::unique_ptr<QListWidgetSceneElement> listItem =
                std::make_unique<QListWidgetSceneElement>(sceneElementList, component);
        listItems.insert(std::make_pair(component->GetAttachedObjectID(), std::move(listItem)));
    }
    return res;
}

bool SceneElementSystem::Unregister(unsigned int oid)
{
    listItems.erase(oid);
    return ISystem::Unregister(oid);
}

void SceneElementSystem::AttachItemList(QListWidget *list)
{
    sceneElementList = list;
}

void SceneElementSystem::SerializeSceneObjects()
{
    auto &sceneSerilizableObject = MG1::Scene::Get();
    sceneSerilizableObject.Clear();
    
    for (const auto &wel: components)
    {
        if (auto el = wel.second.lock())
        {
            if (el->SerializeObject)
                el->SerializeObject(sceneSerilizableObject);
        }
    }
    //MG1::Scene::Get();
}


