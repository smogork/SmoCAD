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
#include "Scene/Systems/SelectableSystem.h"
#include "Scene/Systems/CompositeAwareSystem.h"
#include <list>

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
    systems.put<SelectableSystem>(std::dynamic_pointer_cast<IAbstractSystem>(std::make_shared<SelectableSystem>()));
    systems.put<CompositeAwareSystem>(std::dynamic_pointer_cast<IAbstractSystem>(std::make_shared<CompositeAwareSystem>()));
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
    cursor = nullptr;
    composite = nullptr;
}

void SceneECS::InitSceneObjects()
{
    auto p1 = std::make_shared<Point>(QVector3D(0, 0, 1));
    auto p2 = std::make_shared<Point>(QVector3D(2, 0, 0));
    objects.push_back(p1);
    objects.push_back(p2);
    objects.push_back(std::make_shared<Cube>(QVector3D(2, 2, 0)));
    objects.push_back(std::make_shared<Cube>(QVector3D(2, 4, 0)));
    objects.push_back(std::make_shared<Cube>(QVector3D(2, 6, 2)));
    objects.push_back(std::make_shared<Torus>(QVector3D(5, 1, 5)));

    //composite = std::make_unique<Composite>(p1->p_CompositeAware);
    //composite->AddObject(p2->p_CompositeAware);

    //composite->p_Transform->Rotation.setX(90);
}

void SceneECS::RemoveUniqueObjects()
{
    grid.reset();
    cursor.reset();
    composite.reset();
}

void SceneECS::RemoveObjectsFromScene()
{
    objects.clear();
}

void SceneECS::ClearSystems()
{
    for (auto s : systems)
        s.second->ClearSystem();
}

QString SceneECS::DebugSystemReport()
{
    QString result;
    for (auto s : systems)
        result.append(QString("%1: %2  ").arg(s.second->GetSystemName()).arg(s.second->GetComponentCount()));
    return result;
}

void SceneECS::MouseClicked(std::shared_ptr<SceneMouseClickEvent> event)
{
    if (auto select = GetSystem<SelectableSystem>().lock())
    {
        if (!select->SelectObject(event))
            UpdateCursorObject(event->ClickCenterPlainPoint);
    }
}

void SceneECS::UpdateCursorObject(QVector3D cursorPos)
{
    if (cursor)
        cursor->p_Transform->Position = cursorPos;
    else
        cursor = std::make_unique<Cursor>(cursorPos);
}

