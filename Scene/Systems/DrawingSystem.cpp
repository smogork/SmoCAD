//
// Created by ksm on 4/3/22.
//

#include "DrawingSystem.h"

/*std::weak_ptr<Drawing> DrawingSystem::GetComponent(unsigned int oid)
{
    return std::weak_ptr<Drawing>();
}

std::vector<std::weak_ptr<Drawing>> DrawingSystem::GetComponents()
{
    return std::vector<std::weak_ptr<Drawing>>();
}

std::shared_ptr<Drawing> DrawingSystem::CreateRegistered(unsigned int oid)
{
    return std::shared_ptr<Drawing>();
}

bool DrawingSystem::Unregister(unsigned int oid)
{
    return false;
}*/

void DrawingSystem::Render(QOpenGLContext* context)
{
    for (const std::pair<unsigned int, std::weak_ptr<Drawing>> &p : components)
        if (auto obj = p.second.lock())
            if (obj->Enabled)
                obj->Render(context);
}