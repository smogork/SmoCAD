//
// Created by ksm on 4/3/22.
//

#include "SceneECS.h"
#include "Scene/Systems/DrawingSystem.h"
#include "Scene/Systems/TransformSystem.h"
#include "Scene/Entities/Points/Point.h"
#include "Scene/Entities/Cube.h"
#include "Scene/Systems/UVParamsSystem.h"
#include "Scene/Entities/Torus.h"
#include "Scene/Entities/Cursor.h"
#include "Scene/Systems/SelectableSystem.h"
#include "Scene/Systems/CompositeAwareSystem.h"
#include "Scene/Systems/CollectionAwareSystem.h"
#include "Scene/Systems/TransformCollectionSystem.h"
#include "Scene/Entities/Polyline.h"
#include "Scene/Entities/Curves/VirtualBezierC0.h"
#include "Scene/Entities/Curves/BezierC2.h"
#include "Controls/ComponentControl.h"
#include "Scene/Systems/ScreenSelectableSystem.h"
#include "Scene/Entities/Curves/InterpolationC2.h"
#include <list>

std::shared_ptr<SceneECS> SceneECS::scene = nullptr;

std::weak_ptr<SceneECS> SceneECS::Instance()
{
    if (!SceneECS::scene)
    {
        SceneECS::scene.reset(new SceneECS());
    }
    return SceneECS::scene;
}

SceneECS::SceneECS()
{
    objectCounter = NON_OBJECT_ID + 1;

    systems.put<TransformSystem>(std::dynamic_pointer_cast<IAbstractSystem>(std::make_shared<TransformSystem>()));
    systems.put<DrawingSystem>(std::dynamic_pointer_cast<IAbstractSystem>(std::make_shared<DrawingSystem>()));
    systems.put<UVParamsSystem>(std::dynamic_pointer_cast<IAbstractSystem>(std::make_shared<UVParamsSystem>()));
    systems.put<SelectableSystem>(std::dynamic_pointer_cast<IAbstractSystem>(std::make_shared<SelectableSystem>()));
    systems.put<ScreenSelectableSystem>(
            std::dynamic_pointer_cast<IAbstractSystem>(std::make_shared<ScreenSelectableSystem>()));
    systems.put<CompositeAwareSystem>(
            std::dynamic_pointer_cast<IAbstractSystem>(std::make_shared<CompositeAwareSystem>()));
    systems.put<CollectionAwareSystem>(
            std::dynamic_pointer_cast<IAbstractSystem>(std::make_shared<CollectionAwareSystem>()));
    systems.put<TransformCollectionSystem>(
            std::dynamic_pointer_cast<IAbstractSystem>(std::make_shared<TransformCollectionSystem>()));
    systems.put<SceneElementSystem>(
            std::dynamic_pointer_cast<IAbstractSystem>(std::make_shared<SceneElementSystem>()));
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
    auto p1 = std::make_shared<Point>("P1", QVector3D(0, 0, 0));
    auto p2 = std::make_shared<Point>("P2", QVector3D(1, 1, 0));
    auto p3 = std::make_shared<Point>("P3", QVector3D(5, 0, 3));
    auto p4 = std::make_shared<Point>("P4", QVector3D(-3, 0, 0));
    auto p5 = std::make_shared<Point>("P5", QVector3D(-2, -1, 1));
    auto p6 = std::make_shared<Point>("P6", QVector3D(5, 3, 0));
    auto p7 = std::make_shared<Point>("P7", QVector3D(6, 0, 0));
    auto p8 = std::make_shared<Point>("P8", QVector3D(7, -1, 0));
    auto p9 = std::make_shared<Point>("P9", QVector3D(8, 0, 0));
    objects.push_back(p1);
    objects.push_back(p2);
    objects.push_back(p3);
    objects.push_back(p4);
    objects.push_back(p5);
    objects.push_back(p6);
    objects.push_back(p7);
    objects.push_back(p8);
    objects.push_back(p9);
    objects.push_back(std::make_shared<Cube>("Cube1", QVector3D(2, 2, 0)));
    objects.push_back(std::make_shared<Cube>("Cube2", QVector3D(2, 4, 0)));
    objects.push_back(std::make_shared<Cube>("Cube3", QVector3D(2, 6, 2)));
    objects.push_back(std::make_shared<Torus>("Torus1", QVector3D(10, 1, 10)));


    auto polyline = std::make_shared<InterpolationC2>("InterpolationC2Test");
    polyline->p_Collection->AddPoint(p1->p_CollectionAware);
    polyline->p_Collection->AddPoint(p2->p_CollectionAware);
    polyline->p_Collection->AddPoint(p3->p_CollectionAware);
    polyline->p_Collection->AddPoint(p4->p_CollectionAware);
    polyline->p_Collection->AddPoint(p5->p_CollectionAware);
    polyline->p_Collection->AddPoint(p6->p_CollectionAware);
    /*polyline->p_Collection->AddPoint(p7->p_CollectionAware);
    polyline->p_Collection->AddPoint(p8->p_CollectionAware);
    polyline->p_Collection->AddPoint(p9->p_CollectionAware);*/
    objects.push_back(polyline);

    /*auto c2 = std::make_shared<BezierC2>("BezeierC2WithMultipleKnots");
    c2->p_Collection->AddPoint(p6->p_CollectionAware);
    c2->p_Collection->AddPoint(p1->p_CollectionAware);
    c2->p_Collection->AddPoint(p2->p_CollectionAware);
    c2->p_Collection->AddPoint(p2->p_CollectionAware);
    c2->p_Collection->AddPoint(p3->p_CollectionAware);
    c2->p_Collection->AddPoint(p5->p_CollectionAware);
    objects.push_back(c2);*/
    //p5->p_Transform->Position = QVector3D(-3, -3, 0);


    //composite = std::make_unique<Composite>(p1->p_CompositeAware);
    //composite->AddObject(p2->p_CompositeAware);

    //composite->p_Transform->Rotation.setX(90);
}

void SceneECS::RemoveUniqueObjects()
{
    grid.reset();
    composite.reset();
    cursor.reset();
    emit CursorChange(nullptr);
}

void SceneECS::RemoveObjectsFromScene()
{
    objects.clear();
}

void SceneECS::ClearSystems()
{
    for (auto s: systems)
        s.second->ClearSystem();
}

QString SceneECS::DebugSystemReport()
{
    QString result;
    for (auto s: systems)
        result.append(QString("%1: %2  ").arg(s.second->GetSystemName()).arg(s.second->GetComponentCount()));
    return result;
}

unsigned int SceneECS::MouseClicked(std::shared_ptr<SceneMouseClickEvent> event)
{
    if (event->SelectObjects)
    {
        if (auto screenSelect = GetSystem<ScreenSelectableSystem>().lock())
        {
            auto item = screenSelect->SelectObject(event);
            if (item)
                return item->GetAttachedObjectID();
        }
    }

    UpdateCursorObject(event->ClickCenterPlainPoint);
    if (auto select = GetSystem<SelectableSystem>().lock())
    {
        //select->Unselect();
        if (auto sObj = select->GetSelectedObject())
            return sObj->GetAttachedObjectID();
    }

    return NON_OBJECT_ID;
}

void SceneECS::UpdateCursorObject(QVector3D cursorPos)
{
    if (cursor)
        cursor->p_Transform->Position = cursorPos;
    else
    {
        cursor = std::make_unique<Cursor>(cursorPos);
        emit CursorChange(cursor);
    }
}

void SceneECS::AddObject(std::shared_ptr<IEntity> obj)
{
    auto t = obj->GetComponent<Transform>().lock();
    if (t)
    {
        if (cursor)
        {
            t->Position = cursor->p_Transform->Position.value();
            objects.push_back(obj);
        }
    } else
        objects.push_back(obj);
}

std::list<std::unique_ptr<ComponentControl>> SceneECS::CreateUIForObject(unsigned int oid)
{
    std::list<std::unique_ptr<ComponentControl>> res;

    if (oid == NON_OBJECT_ID)
        return res;

    for (auto s: systems)
    {
        std::unique_ptr<ComponentControl> elem = s.second->PrepareUIForObject(oid);
        if (elem)
            res.push_back(std::move(elem));
    }

    return res;
}

void SceneECS::RemoveObject(unsigned int oid)
{
    objects.remove_if([&](std::shared_ptr<IEntity> &item)
                      {
                          return item->GetObjectID() == oid;
                      }
    );
}

std::list<std::pair<QString, std::function<void(QListWidgetSceneElement *item)> > >
SceneECS::CreateContextMenuForSceneElement(unsigned int oid, int selectionCount)
{
    std::list<std::pair<QString, std::function<void(QListWidgetSceneElement *item)> > > res;

    if (oid == NON_OBJECT_ID)
        return res;

    auto selectedSystem = GetSystem<SelectableSystem>().lock();
    auto selectedObj = selectedSystem->GetSelectedObject();
    if (selectedObj == nullptr)
        return res;

    for (auto s: systems)
    {
        std::list<std::pair<QString, std::function<void(QListWidgetSceneElement *item)> > > inner = s.second
                ->CreateContextMenuForSceneElement(
                        oid, selectedObj->GetAttachedObjectID(), selectionCount);
        for (auto item: inner)
            res.push_back(item);
    }

    return res;
}

void SceneECS::InitializeScene()
{
    InitUniqueObjects();
    InitSceneObjects();
}

