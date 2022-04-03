//
// Created by ksm on 4/3/22.
//

#include "SceneECS.h"

std::shared_ptr<SceneECS> SceneECS::scene = nullptr;

std::weak_ptr<SceneECS> SceneECS::Instance()
{
    if (!SceneECS::scene)
        SceneECS::scene.reset(new SceneECS());
    return SceneECS::scene;
}

SceneECS::SceneECS()
{
    objectCounter = 0;

    systems.put<TransformSystem>(std::make_shared<TransformSystem>());
}

SceneECS::~SceneECS()
{

}

unsigned int SceneECS::GetNewObjectID()
{
    return objectCounter++;
}
