//
// Created by ksm on 4/3/22.
//

#include "SceneECS.h"
#include "Scene/Systems/DrawingSystem.h"
#include "Scene/Systems/TransformSystem.h"
#include "Scene/Entities/Point.h"
#include "Scene/Entities/Cube.h"
#include "Scene/Systems/UVParamsSystem.h"
#include "Scene/Entities/Torus.h"
#include "Scene/Entities/Cursor.h"

std::shared_ptr<SceneECS> SceneECS::scene = nullptr;

std::weak_ptr<SceneECS> SceneECS::Instance()
{
    if (!SceneECS::scene)
    {
        SceneECS::scene.reset(new SceneECS());
        scene->InitUniqueObjects();
        scene->InitSceneObjects();
    }
    return SceneECS::scene;
}

SceneECS::SceneECS()
{
    objectCounter = 0;

    systems.put<TransformSystem>(std::dynamic_pointer_cast<IAbstractSystem>(std::make_shared<TransformSystem>()));
    systems.put<DrawingSystem>(std::dynamic_pointer_cast<IAbstractSystem>(std::make_shared<DrawingSystem>()));
    systems.put<UVParamsSystem>(std::dynamic_pointer_cast<IAbstractSystem>(std::make_shared<UVParamsSystem>()));
}

SceneECS::~SceneECS()
{

}

unsigned int SceneECS::GetNewObjectID()
{
    return objectCounter++;
}

void SceneECS::InitUniqueObjects()
{
    grid = std::make_unique<Grid>();
}

void SceneECS::InitSceneObjects()
{
    objects.push_back(std::make_shared<Point>(QVector3D(0, 0, 1)));
    objects.push_back(std::make_shared<Point>(QVector3D(2, 0, 0)));
    objects.push_back(std::make_shared<Cube>(QVector3D(2, 2, 0)));
    objects.push_back(std::make_shared<Torus>(QVector3D(5, 1, 5)));
    objects.push_back(std::make_shared<Cursor>(QVector3D(0, 0, 0)));
}

void SceneECS::RemoveUniqueObjects()
{
    grid.reset();
}

void SceneECS::RemoveObjectsFromScene()
{
    objects.clear();
}

QString SceneECS::DebugSystemReport()
{
    //[TODO] dodac raport liczby elementow w kazdym systemie
    return QString();
}
