//
// Created by ksm on 4/3/22.
//

#ifndef SMOCAD_IABSTRACTSYSTEM_H
#define SMOCAD_IABSTRACTSYSTEM_H


#include <string>
#include <memory>
#include <QWidget>
#include "Controls/ComponentControl.h"

class IAbstractSystem
{
protected:
    IAbstractSystem() {};

public:
    virtual int GetComponentCount() = 0;
    virtual const char* GetSystemName() = 0;
    virtual void ClearSystem() { };
    virtual std::unique_ptr<ComponentControl> PrepareUIForObject(unsigned int oid) { return nullptr; }
};


#endif //SMOCAD_IABSTRACTSYSTEM_H
