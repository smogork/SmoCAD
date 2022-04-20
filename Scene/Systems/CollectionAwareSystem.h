//
// Created by ksm on 4/9/22.
//

#ifndef SMOCAD_COLLECTIONAWARESYSTEM_H
#define SMOCAD_COLLECTIONAWARESYSTEM_H

#include "ISystem.h"
#include "Scene/Components/CollectionAware.h"

class CollectionAwareSystem: public ISystem<CollectionAware>
{
public:
    CollectionAwareSystem(): ISystem(SYSTEM_ID::COLLECTION_AWARE)
    { }
    const char* GetSystemName() override { return "CollectionAwareSystem"; }

    std::list<std::pair<QString, std::function<void(QListWidgetSceneElement* item)> > >
    CreateContextMenuForSceneElement(unsigned int contextOid, unsigned int selectedOid, int selectionCount) override;

protected:
    void AddObjectToCollection(QListWidgetSceneElement * elem);
};


#endif //SMOCAD_COLLECTIONAWARESYSTEM_H
