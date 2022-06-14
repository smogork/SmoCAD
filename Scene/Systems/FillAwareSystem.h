//
// Created by ksm on 6/13/22.
//

#ifndef SMOCAD_FILLAWARESYSTEM_H
#define SMOCAD_FILLAWARESYSTEM_H

#include "ISystem.h"
#include "Scene/Components/FillAware.h"

class FillAwareSystem: public ISystem<FillAware>
{
public:
    FillAwareSystem(): ISystem(SYSTEM_ID::FILL_AWARE)
    { }
    const char* GetSystemName() override { return "FillAwareSystem"; }
    
    std::list<std::pair<QString, std::function<void(const std::vector<unsigned int> &selectedOids,
                                                    const std::vector<unsigned int> &listContextOids)> > >
    CreateContextMenuItemsForSceneList(const std::vector<unsigned int> &selectedOids,
                                       const std::vector<unsigned int> &listContextOids) override;
    
protected:
    void CreateFillPlane(const std::vector<unsigned int> &listContextOids);
    
    bool IsHoleToFill(const std::vector<std::shared_ptr<FillAware>>& awares);
};

#endif //SMOCAD_FILLAWARESYSTEM_H
