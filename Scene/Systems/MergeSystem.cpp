//
// Created by ksm on 6/12/22.
//

#include "MergeSystem.h"
#include "Scene/Utilities/Utilites.h"
#include "Scene/SceneECS.h"
#include "SelectableSystem.h"
#include "TransformCollectionSystem.h"

std::list<std::pair<QString, std::function<void(const std::vector<unsigned int> &selectedOids)> > >
MergeSystem::CreateContextMenuItemsForScene(const std::vector<unsigned int> &selectedOids)
{
    std::list<std::pair<QString, std::function<void(const std::vector<unsigned int> &selectedOids)> > > res;
    auto filteredObjects = FilterObjects(selectedOids);
    
    if (filteredObjects.size() != 2)
        return res;
    
    res.push_back(
            std::make_pair(
                    "Merge points",
                    ASSIGN_CONTEXT_SCENE_FUNCTION(&MergeSystem::MergeElements)));
    
    return res;
}

void MergeSystem::MergeElements(const std::vector<unsigned int> &selectedOids)
{
    auto filteredObjects = FilterObjects(selectedOids);
    
    if (filteredObjects.size() != 2)
        throw std::runtime_error("Merge impossible - wrong count of items");
    
    std::shared_ptr<Merge> succesor = filteredObjects[0];
    std::shared_ptr<Merge> toReplace = filteredObjects[1];
    
    QVector3D resultLocation = (succesor->p_Transform->Position + toReplace->p_Transform->Position) / 2;

    if (auto scene = SceneECS::Instance().lock())
    {
        //Usuniecie zanaczenia
        auto selSys = scene->GetSystem<SelectableSystem>().lock();
        selSys->Unselect();
    
        //Podmiana obiektow w kolekcjach
        auto colSys = scene->GetSystem<TransformCollectionSystem>().lock();
        colSys->ReplaceObjectInCollections(toReplace->p_CollectionAware, succesor->p_CollectionAware);
        
        //Usuniecie podmianianego obiektu
        scene->RemoveObject(toReplace->GetAttachedObjectID());
        
        //Ustawienie nowej lokalizacji nastepcy
        succesor->p_Transform->Position = resultLocation;
    }
}
