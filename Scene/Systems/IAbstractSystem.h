//
// Created by ksm on 4/3/22.
//

#ifndef SMOCAD_IABSTRACTSYSTEM_H
#define SMOCAD_IABSTRACTSYSTEM_H


#include <string>
#include <memory>

class IAbstractSystem
{
protected:
    IAbstractSystem() {};

public:
    virtual int GetComponentCount() = 0;
    virtual const char* GetSystemName() = 0;
    virtual void ClearSystem() { };
};


#endif //SMOCAD_IABSTRACTSYSTEM_H
