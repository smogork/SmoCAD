//
// Created by ksm on 4/9/22.
//

#ifndef SMOCAD_TRANSFORMCOLLECTIONSYSTEM_H
#define SMOCAD_TRANSFORMCOLLECTIONSYSTEM_H

#include "ISystem.h"
#include "Scene/Components/TransformCollection.h"

class TransformCollectionSystem: public ISystem<TransformCollection>
{
public:
    TransformCollectionSystem(): ISystem(SYSTEM_ID::TRANSFORM_COLLECTION)
    { }
    const char* GetSystemName() override { return "TransformCollectionSystem"; }

    std::unique_ptr<ComponentControl> PrepareUIForObject(unsigned int oid) override;
    std::list<std::pair<QString, std::function<void(const std::vector<unsigned int> &selectedOids)> > >
    CreateContextMenuItemsForScene(const std::vector<unsigned int> &selectedOids) override;
    
    void ReplaceObjectInCollections(std::shared_ptr<CollectionAware> toReplace, std::shared_ptr<CollectionAware> newElement);
    
protected:
    void CreateAndAddPointToCollection(std::shared_ptr<TransformCollection> col);
};


#endif //SMOCAD_TRANSFORMCOLLECTIONSYSTEM_H
