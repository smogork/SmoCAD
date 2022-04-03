//
// Created by ksm on 4/3/22.
//

#ifndef SMOCAD_ISYSTEM_H
#define SMOCAD_ISYSTEM_H

#include <map>
#include <memory>
#include <vector>

enum SYSTEM_ID
{
    TRANSFORM,
    SYSTEM_COUNT
};


class ISystem
{
private:
    unsigned int systemID;

public:
    explicit ISystem(unsigned int sid) { systemID = sid; }

    unsigned int GetSystemID() { return systemID; }

};


#endif //SMOCAD_ISYSTEM_H
