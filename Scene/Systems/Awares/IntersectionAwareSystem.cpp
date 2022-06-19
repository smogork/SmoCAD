//
// Created by ksm on 6/19/22.
//

#include "IntersectionAwareSystem.h"

std::list<std::pair<QString, std::function<void(const std::vector<unsigned int> &selectedOids,
                                                const std::vector<unsigned int> &listContextOids)> > >
IntersectionAwareSystem::CreateContextMenuItemsForSceneList(const std::vector<unsigned int> &selectedOids,
                                                            const std::vector<unsigned int> &listContextOids)
{
    std::list<std::pair<QString, std::function<void(const std::vector<unsigned int> &selectedOids,
                                                    const std::vector<unsigned int> &listContextOids)> > > res;
    auto filteredObjects = FilterObjects(listContextOids);
    
    if (filteredObjects.size() == 2)
        res.push_back(
                std::make_pair(
                        "Calculate intersection",
                        [this, filteredObjects](const std::vector<unsigned int> &selectedOids,
                               const std::vector<unsigned int> &listContextOids)
                        {
                            CreateIntersectionCurveBetween(filteredObjects[0], filteredObjects[1]);
                        }));
    
    if (filteredObjects.size() == 1)
        res.push_back(
                std::make_pair(
                        "Calculate self intersection",
                        [this, filteredObjects](const std::vector<unsigned int> &selectedOids,
                               const std::vector<unsigned int> &listContextOids)
                        {
                            CreateIntersectionCurveBetween(filteredObjects[0], filteredObjects[0]);
                        }));
    
    return res;
}

void IntersectionAwareSystem::CreateIntersectionCurveBetween(std::shared_ptr<IntersectionAware> one, std::shared_ptr<IntersectionAware> two)
{

}
