//
// Created by ksm on 4/3/22.
//

#include "IEntity.h"
#include <fmt/core.h>
#include <QString>

IEntity::IEntity(unsigned int cid)
{
    if (cid >= ENTITY_CLASS::CLASS_COUNT)
        throw std::runtime_error(QString("entity classID %1 unknown").arg(cid).toStdString());

    classID = cid;
}

unsigned int IEntity::GetObjectID()
{
    return objectID;
}

unsigned int IEntity::GetClassID()
{
    return classID;
}
