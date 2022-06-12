//
// Created by ksm on 4/9/22.
//

#include "TransformCollectionSystem.h"
#include "Controls/transformcollectioncontrol.h"
#include "Scene/Entities/Points/Point.h"
#include "Scene/SceneECS.h"
#include "Controls/EntityContextMenu.h"

std::unique_ptr<ComponentControl> TransformCollectionSystem::PrepareUIForObject(unsigned int oid)
{
    if (auto collection = GetComponent(oid).lock())
        return std::move(std::make_unique<TransformCollectionControl>(collection));
    return nullptr;
}

void TransformCollectionSystem::ReplaceObjectInCollections(std::shared_ptr<CollectionAware> toReplace,
                                                           std::shared_ptr<CollectionAware> newElement)
{
    for (auto& wcol : components)
    {
        if (auto col = wcol.second.lock())
            col->ReplaceObject(toReplace->GetAttachedObjectID(), newElement);
    }
}

void TransformCollectionSystem::CreateAndAddPointToCollection(std::shared_ptr<TransformCollection> col)
{
    if (auto scene = SceneECS::Instance().lock())
    {
        std::shared_ptr<Point> p = std::make_shared<Point>("NewPoint");
        scene->AddObject(p);
        col->AddPoint(p->p_CollectionAware);
    
        EntityContextMenu::MakeRepaint();
    }
}

std::list<std::pair<QString, std::function<void(const std::vector<unsigned int> &selectedOids)> > >
TransformCollectionSystem::CreateContextMenuItemsForScene(const std::vector<unsigned int> &selectedOids)
{
    std::list<std::pair<QString, std::function<void(const std::vector<unsigned int> &selectedOids)> > > res;
    auto filteredObjects = FilterObjects(selectedOids);
    
    if (filteredObjects.size() == 1)
    {
        std::shared_ptr<TransformCollection> col = filteredObjects[0];
        res.push_back(
                std::make_pair(
                        "Create and add point to colletion",
                        [this, col](const std::vector<unsigned int> &selectedOids)
                        {
                            CreateAndAddPointToCollection(col);
                        }));
    }
    
    return res;
}
