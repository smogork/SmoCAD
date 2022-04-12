//
// Created by ksm on 4/5/22.
//

#include "UVParams.h"
#include "Scene/Systems/UVParamsSystem.h"
#include "Scene/SceneECS.h"

UVParams::UVParams(unsigned int oid): IComponent(oid, UV)
{
    m_U = 5;
    m_V = 1;
    m_UDensity = 32;
    m_VDensity = 16;
    EmitParamsChangedEvent();//?
}

UVParams::~UVParams()
{
    UnregisterComponent();
}

std::shared_ptr<UVParams> UVParams::CreateRegisteredComponent(unsigned int oid)
{
    if (auto scene = SceneECS::Instance().lock())
    {
        auto system = scene->GetSystem<UVParamsSystem>().lock();
        return system->CreateRegistered(oid);
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

void UVParams::SetU(float val)
{
    //jakies sprawdzenia wartosci
    m_U = val;
    EmitParamsChangedEvent();
}

void UVParams::SetV(float val)
{
//jakies sprawdzenia wartosci
    m_V = val;
    EmitParamsChangedEvent();
}

void UVParams::SetUDensity(int val)
{
//jakies sprawdzenia wartosci
    m_UDensity = val;
    EmitParamsChangedEvent();
}

void UVParams::SetVDensity(int val)
{
//jakies sprawdzenia wartosci
    m_UDensity = val;
    EmitParamsChangedEvent();
}

void UVParams::EmitParamsChangedEvent()
{
    auto event = std::make_shared<UVParamsChanged>(m_U, m_V, m_UDensity, m_VDensity);
    emit ParametersChanged(event);
}
