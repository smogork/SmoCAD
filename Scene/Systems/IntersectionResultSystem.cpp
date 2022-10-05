//
// Created by ksm on 6/28/22.
//

#include "IntersectionResultSystem.h"
#include "Controls/intersectionresultcontrol.h"

std::list<std::pair<QString, std::function<void(const std::vector<unsigned int> &selectedOids,
                                                const std::vector<unsigned int> &listContextOids)> > >
IntersectionResultSystem::CreateContextMenuItemsForSceneList(const std::vector<unsigned int> &selectedOids,
                                                             const std::vector<unsigned int> &listContextOids)
{
    return IAbstractSystem::CreateContextMenuItemsForSceneList(selectedOids, listContextOids);
}

std::unique_ptr<ComponentControl> IntersectionResultSystem::PrepareUIForObject(unsigned int oid)
{
    if (auto inRes = GetComponent(oid).lock())
        return std::move(std::make_unique<IntersectionResultControl>(inRes));
    return nullptr;
}
