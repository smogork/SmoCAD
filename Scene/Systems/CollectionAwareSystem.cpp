//
// Created by ksm on 4/9/22.
//

#include "CollectionAwareSystem.h"
#include "TransformCollectionSystem.h"
#include "Scene/SceneECS.h"
#include "SelectableSystem.h"
#include "Scene/Utilities/Utilites.h"

/*std::list<std::pair<QString, std::function<void(QListWidgetSceneElement *item)> > >
CollectionAwareSystem::CreateContextMenuForSceneElement(unsigned int contextOid, unsigned int selectedOid, int selectionCount)
{
    std::list<std::pair<QString, std::function<void(QListWidgetSceneElement *item)> > > res;

    auto aware = GetComponent(contextOid).lock();
    if (!aware)
        return res;

    //Tymczasowe zabezpieczenie przed dodawaniem wielu punktów jednoczesnie
    //Wymaga przebudowania przekazywanych obiektów do tej funkcji aby zaimplementowane prawidlowo
    if (selectionCount != 1)
        return res;

    if (auto scene = SceneECS::Instance().lock())
    {
        if (auto col = scene->GetComponentOfSystem<TransformCollectionSystem, TransformCollection>(selectedOid).lock())
        {
            if (!col->IsContentLocked())
                res.emplace_back(std::make_pair("Add object to collection",
                                                ASSIGN_CONTEXT_FUNCTION(
                                                        &CollectionAwareSystem::AddObjectToCollection)));
        }
    }

    return res;
}

void CollectionAwareSystem::AddObjectToCollection(const std::vector<unsigned int>& items)
{
    if (auto awareElement = GetComponent(elem->GetAttachedObjectID()).lock())
    {
        if (auto scene = SceneECS::Instance().lock())
        {
            auto selectedSystem = scene->GetSystem<SelectableSystem>().lock();
            auto selectedObj = selectedSystem->GetSelectedObject();
            if (selectedObj == nullptr)
                return;

            scene->GetComponentOfSystem<TransformCollectionSystem, TransformCollection>(
                    selectedObj->GetAttachedObjectID()).lock()->AddPoint(awareElement);
        }
    }
}*/

std::list<std::pair<QString, std::function<void(const std::vector<unsigned int> &selectedOids,
                                                const std::vector<unsigned int> &listContextOids)> > >
CollectionAwareSystem::CreateContextMenuItemsForSceneList(const std::vector<unsigned int> &selectedOids,
                                                          const std::vector<unsigned int> &listContextOids)
{
    return IAbstractSystem::CreateContextMenuItemsForSceneList(selectedOids, listContextOids);
}

std::list<std::pair<QString, std::function<void(const std::vector<unsigned int> &selectedOids)> > >
CollectionAwareSystem::CreateContextMenuItemsForScene(const std::vector<unsigned int> &selectedOids)
{
    return IAbstractSystem::CreateContextMenuItemsForScene(selectedOids);
}

void CollectionAwareSystem::CreateBezierC0(const std::vector<unsigned int> &items)
{

}

void CollectionAwareSystem::CreateBezierC2(const std::vector<unsigned int> &items)
{

}

void CollectionAwareSystem::CreateInterpolationC2(const std::vector<unsigned int> &items)
{

}
