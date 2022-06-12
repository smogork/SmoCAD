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
    std::list<std::pair<QString, std::function<void(QListWidgetSceneElement* item)> > >
    CreateContextMenuForSceneElement(unsigned int contextOid, unsigned int selectedOid, int selectionCount) override;
    
protected:
    void MergeElements(QListWidgetSceneElement * elem);
};

#endif //SMOCAD_MERGESYSTEM_H
