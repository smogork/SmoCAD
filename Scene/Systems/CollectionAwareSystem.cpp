//
// Created by ksm on 4/9/22.
//

#include "CollectionAwareSystem.h"
#include "Scene/SceneECS.h"
#include "Scene/Utilities/Utilites.h"
#include "Scene/Entities/Curves/BezierC0.h"
#include "Scene/Entities/Curves/BezierC2.h"
#include "Scene/Entities/Curves/InterpolationC2.h"
#include "Controls/EntityContextMenu.h"
#include "TransformCollectionSystem.h"

std::list<std::pair<QString, std::function<void(const std::vector<unsigned int> &selectedOids,
                                                const std::vector<unsigned int> &listContextOids)> > >
CollectionAwareSystem::CreateContextMenuItemsForSceneList(const std::vector<unsigned int> &selectedOids,
                                                          const std::vector<unsigned int> &listContextOids)
{
    std::list<std::pair<QString, std::function<void(const std::vector<unsigned int> &selectedOids,
                                                    const std::vector<unsigned int> &listContextOids)> > > res;
    auto filteredObjects = FilterObjects(listContextOids);
    
    if (filteredObjects.empty())
        return res;
    
    if (filteredObjects.size() > 1)
        res.push_back(
                std::make_pair(
                        "Create BezierC0",
                        [this](const std::vector<unsigned int> &selectedOids,
                               const std::vector<unsigned int> &listContextOids)
                        {
                            CreateBezierC0(listContextOids);
                        }));
    if (filteredObjects.size() > 3)
        res.push_back(
                std::make_pair(
                        "Create BezierC2",
                        [this](const std::vector<unsigned int> &selectedOids,
                               const std::vector<unsigned int> &listContextOids)
                        {
                            CreateBezierC2(listContextOids);
                        }));
    
    if (filteredObjects.size() > 1)
        res.push_back(
                std::make_pair(
                        "Create InterpolationC2",
                        [this](const std::vector<unsigned int> &selectedOids,
                               const std::vector<unsigned int> &listContextOids)
                        {
                            CreateInterpolationC2(listContextOids);
                        }));
    
    if (auto scene = SceneECS::Instance().lock())
    {
        if (auto cols = scene->GetSystem<TransformCollectionSystem>().lock())
        {
            auto filteredCols = cols->FilterObjects(selectedOids);
            if (filteredCols.size() == 1)
            {
                std::shared_ptr<TransformCollection> col = filteredCols[0];
                
                if (!col->IsContentLocked())
                res.push_back(
                        std::make_pair(
                                "Add to selected collection",
                                [this, col](const std::vector<unsigned int> &selectedOids,
                                            const std::vector<unsigned int> &listContextOids)
                                {
                                    AddObjectToCollection(col, listContextOids);
                                }));
            }
        }
    }
    
    return res;
}

std::list<std::pair<QString, std::function<void(const std::vector<unsigned int> &selectedOids)> > >
CollectionAwareSystem::CreateContextMenuItemsForScene(const std::vector<unsigned int> &selectedOids)
{
    std::list<std::pair<QString, std::function<void(const std::vector<unsigned int> &selectedOids)> > > res;
    auto filteredObjects = FilterObjects(selectedOids);
    
    if (filteredObjects.empty())
        return res;
    
    if (filteredObjects.size() > 1)
        res.push_back(
                std::make_pair(
                        "Create BezierC0",
                        ASSIGN_CONTEXT_SCENE_FUNCTION(&CollectionAwareSystem::CreateBezierC0)));
    if (filteredObjects.size() > 3)
        res.push_back(
                std::make_pair(
                        "Create BezierC2",
                        ASSIGN_CONTEXT_SCENE_FUNCTION(&CollectionAwareSystem::CreateBezierC2)));
    if (filteredObjects.size() > 1)
        res.push_back(
                std::make_pair(
                        "Create InterpolationC2",
                        ASSIGN_CONTEXT_SCENE_FUNCTION(&CollectionAwareSystem::CreateInterpolationC2)));
    
    return res;
}

void CollectionAwareSystem::CreateBezierC0(const std::vector<unsigned int> &items)
{
    auto filteredObjects = FilterObjects(items);
    
    if (auto scene = SceneECS::Instance().lock())
    {
        std::shared_ptr<BezierC0> b0 = std::make_shared<BezierC0>("NewBezierC0");
        b0->p_Collection->SetPoints(filteredObjects);
        scene->AddObject(b0);
        b0->p_Selected->Selected = true;
        
        EntityContextMenu::MakeControlsUpdate(b0->GetObjectID());
        EntityContextMenu::MakeRepaint();
    }
}

void CollectionAwareSystem::CreateBezierC2(const std::vector<unsigned int> &items)
{
    auto filteredObjects = FilterObjects(items);
    
    if (auto scene = SceneECS::Instance().lock())
    {
        std::shared_ptr<BezierC2> b2 = std::make_shared<BezierC2>("NewBezierC2");
        b2->p_Collection->SetPoints(filteredObjects);
        scene->AddObject(b2);
        b2->p_Selected->Selected = true;
        
        EntityContextMenu::MakeControlsUpdate(b2->GetObjectID());
        EntityContextMenu::MakeRepaint();
    }
}

void CollectionAwareSystem::CreateInterpolationC2(const std::vector<unsigned int> &items)
{
    auto filteredObjects = FilterObjects(items);
    
    if (auto scene = SceneECS::Instance().lock())
    {
        std::shared_ptr<InterpolationC2> i2 = std::make_shared<InterpolationC2>("NewInterpolationC2");
        i2->p_Collection->SetPoints(filteredObjects);
        scene->AddObject(i2);
        i2->p_Selected->Selected = true;
        
        EntityContextMenu::MakeControlsUpdate(i2->GetObjectID());
        EntityContextMenu::MakeRepaint();
    }
}

void CollectionAwareSystem::AddObjectToCollection(std::shared_ptr<TransformCollection> col,
                                                  const std::vector<unsigned int> &items)
{
    auto filteredObjects = FilterObjects(items);
    for (const auto &elem: filteredObjects)
        col->AddPoint(elem);
    EntityContextMenu::MakeRepaint();
}
