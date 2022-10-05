//
// Created by ksm on 4/9/22.
//

#ifndef SMOCAD_COMPOSITEAWARESYSTEM_H
#define SMOCAD_COMPOSITEAWARESYSTEM_H

#include "Scene/Systems/ISystem.h"
#include "Scene/Components/Awares/CompositeAware.h"
#include "Scene/Entities/Composite.h"

class CompositeAwareSystem: public ISystem<CompositeAware>
{
private:
    std::shared_ptr<Composite> composite;
    QPropertyNotifier compositeOutOfContextNotifier;

public:
    CompositeAwareSystem(): ISystem(SYSTEM_ID::COMPOSITE_AWARE)
    { }
    const char* GetSystemName() override { return "CompositeAwareSystem"; }
    void ClearSystem() override;

    unsigned int SelectMultipleObjects(std::list<unsigned int> objectIds);
};


#endif //SMOCAD_COMPOSITEAWARESYSTEM_H
