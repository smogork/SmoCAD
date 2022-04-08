//
// Created by ksm on 4/3/22.
//

#include "IComponent.h"

IComponent::IComponent(unsigned int oid, unsigned sid): attachedObjectId(oid), attachedSystemId(sid)
{
}

IComponent::~IComponent()
{
}

unsigned int IComponent::GetAttachedObjectID()
{
    return attachedObjectId;
}

unsigned int IComponent::GetAttachedSystemID()
{
    return attachedSystemId;
}
