//
// Created by ksm on 4/3/22.
//

#include "SceneECS.h"
#include "Scene/Systems/DrawingSystem.h"
#include "Scene/Systems/TransformSystem.h"
#include "Scene/Entities/Point.h"

std::shared_ptr<SceneECS> SceneECS::scene = nullptr;

std::weak_ptr<SceneECS> SceneECS::Instance()
{
    if (!SceneECS::scene)
    {
        SceneECS::scene.reset(new SceneECS());
        scene->InitObjects();
    }
    return SceneECS::scene;
}

SceneECS::SceneECS()
{
    objectCounter = 0;

    systems.put<TransformSystem>(std::dynamic_pointer_cast<IAbstractSystem>(std::make_shared<TransformSystem>()));
    systems.put<DrawingSystem>(std::dynamic_pointer_cast<IAbstractSystem>(std::make_shared<DrawingSystem>()));


}

SceneECS::~SceneECS()
{

}

unsigned int SceneECS::GetNewObjectID()
{
    return objectCounter++;
}

void SceneECS::InitObjects()
{
    objects.push_back(std::make_shared<Point>(QVector3D(1, 0, 0)));
    objects.push_back(std::make_shared<Point>(QVector3D(1, 1, 0)));
    objects.push_back(std::make_shared<Point>(QVector3D(1, 1, 1)));
}
