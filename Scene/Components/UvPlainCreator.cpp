//
// Created by ksm on 5/17/22.
//

#include "UvPlainCreator.h"
#include "Scene/SceneECS.h"
#include "Scene/Systems/UvPlaneCreatorSystem.h"

UVPlaneCreator::UVPlaneCreator(unsigned int oid) : IComponent(oid, UV_PLANE_CREATOR)
{
    U = 5;
    V = 1;
    UDensity = 32;
    VDensity = 16;
}

UVPlaneCreator::~UVPlaneCreator()
{
    UnregisterComponent();
}

std::shared_ptr<UVPlaneCreator>
UVPlaneCreator::CreateRegisteredComponent(unsigned int oid, std::shared_ptr<Transform> transform, int U, int V)
{
    if (auto scene = SceneECS::Instance().lock())
    {
        auto system = scene->GetSystem<UVPlaneCreatorSystem>().lock();
        auto res = system->CreateRegistered(oid);
        res->U = U;
        res->V = V;
        res->m_transform = transform;
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

std::shared_ptr<PlainC0> UVPlaneCreator::CreatePlainC0()
{
    return std::shared_ptr<PlainC0>();
}
