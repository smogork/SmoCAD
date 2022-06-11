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

class IAbstractSystem: public QObject
{
    Q_OBJECT
protected:
    IAbstractSystem()
    {};

public:
    virtual int GetComponentCount() = 0;
    virtual const char *GetSystemName() = 0;

    virtual void ClearSystem()
    {};

    virtual std::unique_ptr<ComponentControl> PrepareUIForObject(unsigned int oid)
    { return nullptr; }

    virtual std::list<std::pair<QString, std::function<void(QListWidgetSceneElement *item)> > >
    CreateContextMenuForSceneElement(unsigned int contextOid, unsigned int selectedOid, int selectionCount)
    {
        return {};
    }

    virtual void UpdateObjectId(uint oid, uint new_oid)
    { }
};


#endif //SMOCAD_IABSTRACTSYSTEM_H
