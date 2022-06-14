//
// Created by ksm on 6/13/22.
//

#include "FillAwareSystem.h"
#include "Mathematics/PointShapes.h"
#include "Scene/Entities/Points/Point.h"
#include "Scene/SceneECS.h"
#include "Scene/Entities/Planes/FillPlane.h"
#include "Controls/EntityContextMenu.h"

#include <QString>

std::list<std::pair<QString, std::function<void(const std::vector<unsigned int> &selectedOids,
                                                const std::vector<unsigned int> &listContextOids)> > >
FillAwareSystem::CreateContextMenuItemsForSceneList(const std::vector<unsigned int> &selectedOids,
                                                    const std::vector<unsigned int> &listContextOids)
{
    std::list<std::pair<QString, std::function<void(const std::vector<unsigned int> &selectedOids,
                                                    const std::vector<unsigned int> &listContextOids)> > > res;
    auto filteredObjects = FilterObjects(listContextOids);
    
    if (IsHoleToFill(filteredObjects))
        res.push_back(
                std::make_pair(
                        "Fill hole",
                        [this](const std::vector<unsigned int> &selectedOids,
                               const std::vector<unsigned int> &listContextOids)
                        {
                            CreateFillPlane(listContextOids);
                        }));
    
    return res;
}

void FillAwareSystem::CreateFillPlane(const std::vector<unsigned int> &listContextOids)
{
    auto awares = FilterObjects(listContextOids);
    
    if (auto scene = SceneECS::Instance().lock())
    {
        std::shared_ptr<FillPlane> p = std::make_shared<FillPlane>("NewFillPlane", awares);
        scene->AddObject(p);
        p->p_Selected->Selected = true;
    
        EntityContextMenu::MakeControlsUpdate(p->GetObjectID());
        EntityContextMenu::MakeRepaint();
    }
}

bool FillAwareSystem::IsHoleToFill(const std::vector<std::shared_ptr<FillAware>> &awares)
{
    if (awares.size() != 3)
        return false;
    
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
        {
            if (i != j and awares[i]->GetCrossingPointWith(awares[j]) == CrossingPoint::NONE)
                return false;
                
        }
        
    return true;
}
