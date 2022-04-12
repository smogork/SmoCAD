//
// Created by ksm on 4/9/22.
//

#ifndef SMOCAD_COMPOSITEAWARESYSTEM_H
#define SMOCAD_COMPOSITEAWARESYSTEM_H

#include "Scene/Systems/ISystem.h"
#include "Scene/Components/CompositeAware.h"


class CompositeAwareSystem: public ISystem<CompositeAware>
{
public:
    CompositeAwareSystem(): ISystem(SYSTEM_ID::COMPOSITE_AWARE)
    { }
    const char* GetSystemName() override { return "CompositeAwareSystem"; }
};


#endif //SMOCAD_COMPOSITEAWARESYSTEM_H
