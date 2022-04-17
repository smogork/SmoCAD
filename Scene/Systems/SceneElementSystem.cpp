//
// Created by ksm on 4/13/22.
//

#include "SceneElementSystem.h"
#include "Scene/SceneECS.h"

#pragma region QListWidgetSceneElement

SceneElementSystem::QListWidgetSceneElement::QListWidgetSceneElement(QListWidget *parent,
                                                                     std::shared_ptr<SceneElement> element)
        : QListWidgetItem(element->Name.value(), parent), element(element)
{
    nameNotifier = element->Name.addNotifier([this]()
                                             {
                                                 if (auto elem = this->element.lock())
                                                     this->setText(elem->Name);
                                             });
}

void SceneElementSystem::QListWidgetSceneElement::SelectItem()
{
    if (auto elem = element.lock())
    {
        elem->p_Selected->Selected = true;
    }
}

unsigned int SceneElementSystem::QListWidgetSceneElement::GetAttachedObjectID()
{
    if (auto elem = element.lock())
        return elem->GetAttachedObjectID();
    return SceneECS::NON_OBJECT_ID;
}

void SceneElementSystem::QListWidgetSceneElement::Rename(const QString &name)
{
    if (auto elem = element.lock())
        elem->Name = name;
}

const QString SceneElementSystem::QListWidgetSceneElement::GetName()
{
    if (auto elem = element.lock())
        return elem->Name;
    return "";
}

#pragma endregion

std::shared_ptr<SceneElement> SceneElementSystem::CreateRegistered(unsigned int oid)
{
    auto item = ISystem::CreateRegistered(oid);

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


