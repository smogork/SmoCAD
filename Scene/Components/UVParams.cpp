//
// Created by ksm on 4/5/22.
//

#include "UVParams.h"
#include "Scene/Systems/UVParamsSystem.h"
#include "Scene/SceneECS.h"

UVParams::UVParams(unsigned int oid): IComponent(oid, UV)
{
    U = 5;
    V = 1;
    UDensity = 32;
    VDensity = 16;
}

UVParams::~UVParams()
{
    UnregisterComponent();
}

std::shared_ptr<UVParams> UVParams::CreateRegisteredComponent(unsigned int oid, int U, int V)
{
    if (auto scene = SceneECS::Instance().lock())
    {
        auto system = scene->GetSystem<UVParamsSystem>().lock();
       auto res =  system->CreateRegistered(oid);
       res->U = U;
       res->V = V;
       return res;
    }
    return nullptr;
}

void UVParams::UnregisterComponent()
{
    if (auto scene = SceneECS::Instance().lock())
    {
        if (auto system = scene->GetSystem<UVParamsSystem>().lock())
            system->Unregister(GetAttachedObjectID());
    }
}

void UVParams::LockEditUV(bool state)
{
    m_locked = state;
}

bool UVParams::IsUVLocked()
{
    return m_locked;
}
