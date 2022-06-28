//
// Created by ksm on 6/28/22.
//

#include "IntersectionResultSystem.h"

std::list<std::pair<QString, std::function<void(const std::vector<unsigned int> &selectedOids,
                                                const std::vector<unsigned int> &listContextOids)> > >
IntersectionResultSystem::CreateContextMenuItemsForSceneList(const std::vector<unsigned int> &selectedOids,
                                                             const std::vector<unsigned int> &listContextOids)
{
    return IAbstractSystem::CreateContextMenuItemsForSceneList(selectedOids, listContextOids);
}
