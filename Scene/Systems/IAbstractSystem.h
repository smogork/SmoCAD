//
// Created by ksm on 4/3/22.
//

#ifndef SMOCAD_IABSTRACTSYSTEM_H
#define SMOCAD_IABSTRACTSYSTEM_H

#include <string>
#include <memory>
#include <QWidget>
#include "Controls/ListElements/QListWidgetSceneElement.h"
#include "Controls/ComponentControl.h"

class IAbstractSystem : public QObject
{
Q_OBJECT
protected:
    IAbstractSystem(): QObject(nullptr) {}

public:
    virtual int GetComponentCount() = 0;
    virtual const char *GetSystemName() = 0;
    
    virtual void ClearSystem() {};
    
    virtual std::unique_ptr<ComponentControl> PrepareUIForObject(unsigned int oid) { return nullptr; }
    
    virtual std::list<std::pair<QString, std::function<void(const std::vector<unsigned int> &selectedOids)> > >
    CreateContextMenuItemsForScene(const std::vector<unsigned int> &selectedOids)
    {
        return {};
    }
    
    virtual std::list<std::pair<QString, std::function<void(const std::vector<unsigned int> &selectedOids,
                                                            const std::vector<unsigned int> &listContextOids)> > >
    CreateContextMenuItemsForSceneList(const std::vector<unsigned int> &selectedOids,
                                       const std::vector<unsigned int> &listContextOids)
    {
        return {};
    }
    
    virtual void UpdateObjectId(uint oid, uint new_oid) {}
};

#endif //SMOCAD_IABSTRACTSYSTEM_H
