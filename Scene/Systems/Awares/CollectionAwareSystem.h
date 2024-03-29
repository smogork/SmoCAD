//
// Created by ksm on 4/9/22.
//

#ifndef SMOCAD_COLLECTIONAWARESYSTEM_H
#define SMOCAD_COLLECTIONAWARESYSTEM_H

#include "Scene/Systems/ISystem.h"
#include "Scene/Components/Awares/CollectionAware.h"
#include "Scene/Components/TransformCollection.h"

class CollectionAwareSystem: public ISystem<CollectionAware>
{
public:
    CollectionAwareSystem(): ISystem(SYSTEM_ID::COLLECTION_AWARE)
    { }
    const char* GetSystemName() override { return "CollectionAwareSystem"; }

    std::list<std::pair<QString, std::function<void(const std::vector<unsigned int> &selectedOids)> > >
    CreateContextMenuItemsForScene(const std::vector<unsigned int> &selectedOids) override;
    std::list<std::pair<QString, std::function<void(const std::vector<unsigned int> &selectedOids,
                                                    const std::vector<unsigned int> &listContextOids)> > >
    CreateContextMenuItemsForSceneList(const std::vector<unsigned int> &selectedOids,
                                       const std::vector<unsigned int> &listContextOids) override;

protected:
    void AddObjectToCollection(std::shared_ptr<TransformCollection> col, const std::vector<unsigned int>& items);
    
    void CreateBezierC0(const std::vector<unsigned int>& items);
    void CreateBezierC2(const std::vector<unsigned int>& items);
    void CreateInterpolationC2(const std::vector<unsigned int>& items);
};


#endif //SMOCAD_COLLECTIONAWARESYSTEM_H
