//
// Created by ksm on 4/13/22.
//

#include "SceneElement.h"
#include "Scene/SceneECS.h"
#include "Scene/Systems/SceneElementSystem.h"

std::shared_ptr<SceneElement>
SceneElement::CreateRegisteredComponent(unsigned int oid, const QString& name, std::shared_ptr<Selectable> select)

{
    if (auto scene = SceneECS::Instance().lock())
    {
        auto system = scene->GetSystem<SceneElementSystem>().lock();
        auto item = system->CreateRegistered(oid);
        item->p_Selected = select;
        item->Name = name;
        return item;
    }
    return nullptr;
}

void SceneElement::UnregisterComponent()
{
    if (auto scene = SceneECS::Instance().lock())
    {
        auto system = scene->GetSystem<SceneElementSystem>().lock();
        system->Unregister(GetAttachedObjectID());
    }
}

SceneElement::SceneElement(unsigned int oid) : IComponent(oid, SCENE_ELEMENT)
{

}

SceneElement::~SceneElement()
{
    UnregisterComponent();
}
