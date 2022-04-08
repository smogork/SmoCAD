//
// Created by ksm on 4/7/22.
//

#ifndef SMOCAD_SELECTABLESYSTEM_H
#define SMOCAD_SELECTABLESYSTEM_H

#include "ISystem.h"
#include "Scene/Components/Selectable.h"

class SelectableSystem: public ISystem<Selectable>
{
public:
    SelectableSystem(): ISystem(SYSTEM_ID::SELECTABLE)
    { }
    const char* GetSystemName() override { return "SelectableSystem"; }
};


#endif //SMOCAD_SELECTABLESYSTEM_H
