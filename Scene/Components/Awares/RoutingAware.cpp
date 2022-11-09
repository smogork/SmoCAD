//
// Created by ksm on 11/9/22.
//

#include "RoutingAware.h"
#include "Scene/Systems/Awares/RoutingAwareSystem.h"
#include "Scene/SceneECS.h"

std::shared_ptr<RoutingAware>
RoutingAware::CreateRegisteredComponent(unsigned int oid, std::shared_ptr<UVParams> uv, std::shared_ptr<Drawing> draw)
{
    if (auto scene = SceneECS::Instance().lock())
    {
        if (auto system = scene->GetSystem<RoutingAwareSystem>().lock())
        {
            auto item = system->CreateRegistered(oid);
            item->p_Drawing = draw;
            item->p_UV = uv;
            return item;
        }
    }
    return nullptr;
}

void RoutingAware::UnregisterComponent()
{
    if (auto scene = SceneECS::Instance().lock())
    {
        auto system = scene->GetSystem<RoutingAwareSystem>().lock();
        system->Unregister(GetAttachedObjectID());
    }
}

RoutingAware::RoutingAware(unsigned int oid) : IComponent(oid, ROUTING_AWARE)
{

}

RoutingAware::~RoutingAware()
{
    UnregisterComponent();
}

void RoutingAware::StartHeighmapRendering()
{
    m_heightmapRendering = true;


}

void RoutingAware::FinishHeighmapRendering()
{
    m_heightmapRendering = false;

    Renderer::UpdateShaders();
}

bool RoutingAware::IsHeightmapRendering()
{
    return m_heightmapRendering;
}
