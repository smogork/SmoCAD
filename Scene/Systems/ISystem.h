//
// Created by ksm on 4/3/22.
//

#ifndef SMOCAD_ISYSTEM_H
#define SMOCAD_ISYSTEM_H

#include <map>
#include <memory>


template<typename C>
class ISystem
{
protected:
    unsigned int systemID;

private:
    std::map<unsigned int, std::shared_ptr<C>> components;

public:
    ISystem(unsigned int sid) { systemID = sid; }

    std::shared_ptr<C> GetComponent(unsigned int oid)
    {

    }

    bool Register(std::shared_ptr<C> component)
    {

    }

    bool Unregister(std::shared_ptr<C> component)
    {

    }
};


#endif //SMOCAD_ISYSTEM_H
