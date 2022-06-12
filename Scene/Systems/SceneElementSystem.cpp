//
// Created by ksm on 4/13/22.
//

#include <QInputDialog>
#include "SceneElementSystem.h"
#include "Scene/SceneECS.h"
#include "Scene/Scene.h"
#include "Scene/Entities/Curves/BezierC2.h"
#include "Scene/Entities/Curves/BezierC0.h"
#include "CollectionAwareSystem.h"
#include "Scene/Entities/Curves/InterpolationC2.h"
#include "Controls/EntityContextMenu.h"
#include "Scene/Utilities/Utilites.h"

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

/*std::unique_ptr<QMenu> SceneElementSystem::CreateContextMenuForSelection()
{
    if (!sceneElementList or sceneElementList->selectedItems().size() == 0)
        return nullptr;
    
    auto item = (QListWidgetSceneElement *) (*sceneElementList->selectedItems().begin());
    unsigned int oid = item->GetAttachedObjectID();
    
    // Create menu and insert some actions

    if (auto scene = SceneECS::Instance().lock())
    {
        myMenu->addAction("Remove", this, &SceneElementSystem::onRemoveSceneElement);
        
        if (sceneElementList->selectedItems().size() == 1)
        {
            myMenu->addAction("Rename", this, &SceneElementSystem::onRenameSceneElement);
        } else if (sceneElementList->selectedItems().size() > 1)
        {
            //[TODO] wyprowadzic to do systemu z kolekcjami - wymag aprzebudowania sposobu przekazywania zaznaczonych obiektów
            myMenu->addAction("Create BezierC0 from points", this, &SceneElementSystem::CreateBezierC0);
            myMenu->addAction("Create BezierC2 from points", this, &SceneElementSystem::CreateBezierC2);
            myMenu->addAction("Create InterpolationC2 from points", this, &SceneElementSystem::CreateInterpolationC2);
        }
        
        auto menu_items = scene->CreateContextMenuForSceneElement(oid, sceneElementList->selectedItems().size());
        for (const std::pair<QString, std::function<void(QListWidgetSceneElement *item)> > &menu_item: menu_items)
        {
            const std::function<void(QListWidgetSceneElement *item)> &func = menu_item.second;
            myMenu->addAction(menu_item.first, this, [func, this]()
            {
                auto item = (QListWidgetSceneElement *) (*sceneElementList->selectedItems().begin());
                
                if (func)
                    func(item);
                
                //emit RequestControlsUpdate(item->GetAttachedObjectID());
                emit RequestRepaint();
            });
        }
    }
    
    return myMenu;
}

void SceneElementSystem::onRemoveSceneElement()
{

}

void SceneElementSystem::onRenameSceneElement()
{

}

void SceneElementSystem::CreateBezierC0()
{
    if (auto scene = SceneECS::Instance().lock())
    {
        std::shared_ptr<BezierC0> b0 = std::make_shared<BezierC0>("NewBezierC0");
        
        int itemsAdded = 0;
        for (QListWidgetItem *gElem: sceneElementList->selectedItems())
        {
            auto item = (QListWidgetSceneElement *) gElem;
            if (auto colElem = scene
                    ->GetComponentOfSystem<CollectionAwareSystem, CollectionAware>(item->GetAttachedObjectID()).lock())
            {
                b0->p_Collection->AddPoint(colElem);
                itemsAdded++;
            }
        }
        
        if (itemsAdded)
        {
            scene->AddObject(b0);
            
            b0->p_Selected->Selected = true;
            emit RequestControlsUpdate(b0->GetObjectID());
            emit RequestRepaint();
        }
    }
}

void SceneElementSystem::CreateBezierC2()
{
    if (auto scene = SceneECS::Instance().lock())
    {
        std::shared_ptr<BezierC2> b2 = std::make_shared<BezierC2>("NewBezierC2");
        
        int itemsAdded = 0;
        for (QListWidgetItem *gElem: sceneElementList->selectedItems())
        {
            auto item = (QListWidgetSceneElement *) gElem;
            if (auto colElem = scene
                    ->GetComponentOfSystem<CollectionAwareSystem, CollectionAware>(item->GetAttachedObjectID()).lock())
            {
                b2->p_Collection->AddPoint(colElem);
                itemsAdded++;
            }
        }
        
        if (itemsAdded)
        {
            scene->AddObject(b2);
            
            b2->p_Selected->Selected = true;
            emit RequestControlsUpdate(b2->GetObjectID());
            emit RequestRepaint();
        }
    }
}

void SceneElementSystem::CreateInterpolationC2()
{
    if (auto scene = SceneECS::Instance().lock())
    {
        std::shared_ptr<InterpolationC2> i2 = std::make_shared<InterpolationC2>("NewInterpolationC2");
        
        int itemsAdded = 0;
        for (QListWidgetItem *gElem: sceneElementList->selectedItems())
        {
            auto item = (QListWidgetSceneElement *) gElem;
            if (auto colElem = scene
                    ->GetComponentOfSystem<CollectionAwareSystem, CollectionAware>(item->GetAttachedObjectID()).lock())
            {
                i2->p_Collection->AddPoint(colElem);
                itemsAdded++;
            }
        }
        
        if (itemsAdded)
        {
            scene->AddObject(i2);
            
            i2->p_Selected->Selected = true;
            emit RequestControlsUpdate(i2->GetObjectID());
            emit RequestRepaint();
        }
    }
}*/

std::vector<unsigned int> SceneElementSystem::GetSelectedItemsOnList()
{
    std::vector<unsigned int> res;
    
    for (const auto &el: sceneElementList->selectedItems())
    {
        auto item = (QListWidgetSceneElement *) el;
        res.push_back(item->GetAttachedObjectID());
    }
    
    return res;
}

std::list<std::pair<QString, std::function<void(const std::vector<unsigned int> &selectedOids)> > >
SceneElementSystem::CreateContextMenuItemsForScene(const std::vector<unsigned int> &selectedOids)
{
    std::list<std::pair<QString, std::function<void(const std::vector<unsigned int> &selectedOids)> > > res;
    auto filteredObjects = FilterObjects(selectedOids);
    
    if (filteredObjects.empty())
        return res;
    
    if (filteredObjects.size() == 1)
        res.push_back(
                std::make_pair(
                        "Rename object",
                        [this](const std::vector<unsigned int> &selectedOids)
                        {
                            auto filteredObjects = FilterObjects(selectedOids);
                            RenameSceneElement(filteredObjects[0]);
                        }));
    
    if (!filteredObjects.empty())
        std::make_pair(
                "Remove objects",
                ASSIGN_CONTEXT_SCENE_FUNCTION(&SceneElementSystem::RemoveSceneElement));
    
    return res;
}

std::list<std::pair<QString, std::function<void(const std::vector<unsigned int> &selectedOids,
                                                const std::vector<unsigned int> &listContextOids)> > >
SceneElementSystem::CreateContextMenuItemsForSceneList(const std::vector<unsigned int> &selectedOids,
                                                       const std::vector<unsigned int> &listContextOids)
{
    std::list<std::pair<QString, std::function<void(const std::vector<unsigned int> &selectedOids,
                                                    const std::vector<unsigned int> &listContextOids)> > > res;
    auto filteredObjects = FilterObjects(listContextOids);
    
    if (filteredObjects.empty())
        return res;
    
    if (filteredObjects.size() == 1)
        res.push_back(
                std::make_pair(
                        "Rename object",
                        [this](const std::vector<unsigned int> &selectedOids,
                               const std::vector<unsigned int> &listContextOids)
                        {
                            auto filteredObjects = FilterObjects(listContextOids);
                            RenameSceneElement(filteredObjects[0]);
                        }));
    
    if (!filteredObjects.empty())
        res.push_back(
                std::make_pair(
                        "Remove objects",
                        [this](const std::vector<unsigned int> &selectedOids,
                               const std::vector<unsigned int> &listContextOids)
                        {
                            RemoveSceneElement(listContextOids);
                        }));
    
    return res;
}

void SceneElementSystem::RemoveSceneElement(const std::vector<unsigned int> &items)
{
    auto filteredObjects = FilterObjectIds(items);
    if (auto scene = SceneECS::Instance().lock())
    {
        for (unsigned int oid: filteredObjects)
            scene->RemoveObject(oid);
    }
    
    EntityContextMenu::MakeControlsUpdate(SceneECS::NON_OBJECT_ID);
    EntityContextMenu::MakeRepaint();
}

void SceneElementSystem::RenameSceneElement(std::shared_ptr<SceneElement> toRename)
{
    bool ok;
    QString newName = QInputDialog::getText(nullptr, "Rename object", "Insert new name of object", QLineEdit::Normal,
                                            toRename->Name, &ok);
    if (!ok)
        return;
    toRename->Name = newName;
}




