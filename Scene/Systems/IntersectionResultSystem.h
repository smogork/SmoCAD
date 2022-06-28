//
// Created by ksm on 6/28/22.
//

#ifndef SMOCAD_INTERSECTIONRESULTSYSTEM_H
#define SMOCAD_INTERSECTIONRESULTSYSTEM_H

#include "Scene/Systems/ISystem.h"
#include "Scene/Components/IntersectionResult.h"

class IntersectionResultSystem : public ISystem<IntersectionResult>
{
public:
    IntersectionResultSystem() : ISystem(SYSTEM_ID::INTERSECTION_RESULT)
    {}

    const char *GetSystemName() override
    { return "IntersectionResultSystem"; }

    std::list<std::pair<QString, std::function<void(const std::vector<unsigned int> &selectedOids,
                                                    const std::vector<unsigned int> &listContextOids)> > >
    CreateContextMenuItemsForSceneList(const std::vector<unsigned int> &selectedOids,
                                       const std::vector<unsigned int> &listContextOids) override;
    std::unique_ptr<ComponentControl> PrepareUIForObject(unsigned int oid) override;
};


#endif //SMOCAD_INTERSECTIONRESULTSYSTEM_H
