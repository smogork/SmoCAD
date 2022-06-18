//
// Created by ksm on 6/12/22.
//

#ifndef SMOCAD_MERGESYSTEM_H
#define SMOCAD_MERGESYSTEM_H

#include "Scene/Components/Merge.h"
#include "ISystem.h"

class MergeSystem : public ISystem<Merge>
{
public:
    MergeSystem(): ISystem(SYSTEM_ID::MERGE)
    { }

    const char* GetSystemName() override { return "MergeSystem"; }
    std::list<std::pair<QString, std::function<void(const std::vector<unsigned int> &selectedOids)> > >
    CreateContextMenuItemsForScene(const std::vector<unsigned int> &selectedOids) override;
    
protected:
    void MergeElements(const std::vector<unsigned int> &selectedOids);
};

#endif //SMOCAD_MERGESYSTEM_H
