//
// Created by ksm on 4/3/22.
//

#include "Drawing.h"
#include "Scene/SceneECS.h"
#include "Scene/Systems/DrawingSystem.h"

std::shared_ptr<Drawing> Drawing::CreateRegisteredComponent(unsigned int oid)
{
    if (auto scene = SceneECS::Instance().lock())
    {
        if (auto system = scene->GetSystem<DrawingSystem>().lock())
            return system->CreateRegistered(oid);
    }
    return nullptr;
}

void Drawing::UnregisterComponent()
{
    if (auto scene = SceneECS::Instance().lock())
    {
        if (auto system = scene->GetSystem<DrawingSystem>().lock())
            system->Unregister(GetAttachedObjectID());
    }
}

Drawing::~Drawing()
{
    UnregisterComponent();
}

Drawing::Drawing(unsigned int oid): IComponent(oid, SYSTEM_ID::DRAWING)
{

}
