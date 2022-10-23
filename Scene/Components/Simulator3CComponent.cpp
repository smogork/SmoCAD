//
// Created by ksm on 10/23/22.
//

#include "Simulator3CComponent.h"
#include "Scene/SceneECS.h"
#include "Scene/Systems/Simulator3CSystem.h"

std::shared_ptr<Simulator3CComponent>
Simulator3CComponent::CreateRegisteredComponent(unsigned int oid, std::shared_ptr<Transform> trans)
{
    if (auto scene = SceneECS::Instance().lock())
    {
        auto system = scene->GetSystem<Simulator3CSystem>().lock();
        auto item = system->CreateRegistered(oid, trans);
        return item;
    }
    return nullptr;
}

Simulator3CComponent::Simulator3CComponent(unsigned int oid) : Simulator3CComponent(oid, nullptr) {}

void Simulator3CComponent::UnregisterComponent()
{
    if (auto scene = SceneECS::Instance().lock())
    {
        auto system = scene->GetSystem<Simulator3CSystem>().lock();
        system->Unregister(GetAttachedObjectID());
    }
}

Simulator3CComponent::Simulator3CComponent(unsigned int oid, std::shared_ptr<Transform> simulatorTransform)
        : IComponent(oid, SIMULATOR3C), p_Transform(simulatorTransform)
{
    m_blockParams = {3, 10, 10, 2048, 2048, 64, 64};//domyslne parametry
    m_blockLower = std::make_unique<BlockLowerWall>(QVector3D(), p_Transform, m_blockParams.WidthX,
                                                    m_blockParams.WidthY);
    m_blockSide = std::make_unique<BlockSideWall>(QVector3D(), p_Transform, m_blockParams.WidthX, m_blockParams.WidthY,
                                                  m_blockParams.Height, m_blockParams.VertexWidthX,
                                                  m_blockParams.VertexWidthY);
    m_blockUpper = std::make_unique<BlockUpperWall>(QVector3D(0, m_blockParams.Height, 0), p_Transform,
                                                    m_blockParams.WidthX, m_blockParams.WidthY,
                                                    m_blockParams.VertexWidthX, m_blockParams.VertexWidthY);
}

Simulator3CComponent::~Simulator3CComponent()
{
    UnregisterComponent();
}

