//
// Created by ksm on 5/17/22.
//

#include "UvPlaneCreator.h"
#include "Scene/SceneECS.h"
#include "Scene/Systems/UvPlaneCreatorSystem.h"
#include "Scene/Entities/Planes/PlaneC0.h"

UVPlaneCreator::UVPlaneCreator(unsigned int oid) : IComponent(oid, UV_PLANE_CREATOR)
{
    U = 5;
    V = 1;
    UDensity = 32;
    VDensity = 16;
    Width = 3;
    Height = 2;
}

UVPlaneCreator::~UVPlaneCreator()
{
    UnregisterComponent();
}

std::shared_ptr<UVPlaneCreator>
UVPlaneCreator::CreateRegisteredComponent(unsigned int oid, std::shared_ptr<Transform> transform, uint cid, int U, int V)
{
    if (auto scene = SceneECS::Instance().lock())
    {
        auto system = scene->GetSystem<UVPlaneCreatorSystem>().lock();
        auto res = system->CreateRegistered(oid);
        res->U = U;
        res->V = V;
        res->m_transform = transform;
        res->m_creatingCid = cid;
        return res;
    }
    return nullptr;
}

void UVPlaneCreator::UnregisterComponent()
{
    if (auto scene = SceneECS::Instance().lock())
    {
        if (auto system = scene->GetSystem<UVPlaneCreatorSystem>().lock())
            system->Unregister(GetAttachedObjectID());
    }
}
