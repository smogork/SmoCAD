//
// Created by ksm on 4/3/22.
//

#include "SceneECS.h"
#include "Scene/Systems/DrawingSystem.h"
#include "Scene/Systems/TransformSystem.h"
#include "Scene/Systems/UVParamsSystem.h"
#include "Scene/Entities/Cursor.h"
#include "Scene/Systems/SelectableSystem.h"
#include "Scene/Systems/CompositeAwareSystem.h"
#include "Scene/Systems/CollectionAwareSystem.h"
#include "Scene/Systems/TransformCollectionSystem.h"
#include "Controls/ComponentControl.h"
#include "Scene/Systems/ScreenSelectableSystem.h"
#include "Scene/Entities/Planes/PlaneCreator.h"
#include "Scene/Systems/UvPlaneCreatorSystem.h"
#include "Serializer.h"
#include "Scene/Entities/Torus.h"
#include "Scene/Entities/Curves/BezierC0.h"
#include "Scene/Entities/Curves/InterpolationC2.h"
#include "Scene/Entities/Curves/BezierC2.h"
#include "Scene/Entities/Planes/PlaneC2.h"
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
    systems.put<UVPlaneCreatorSystem>(
            std::dynamic_pointer_cast<IAbstractSystem>(std::make_shared<UVPlaneCreatorSystem>()));
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
    m_cleanup = true;
    objects.clear();
    m_cleanup = false;
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
    if (!obj)
        return;

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

void SceneECS::AddObjectExplicitPosition(std::shared_ptr<IEntity> obj)
{
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
    //Gdy czyscimy cala scene to niektore obiekty maja logike do niszczenia sie przy zmianie elementow
    //To psuje wewnetrzna logike fukcji clear na mapie, gdy w miedzyczasie sprobujemy susnac jakis element
    if (m_cleanup)
        return;

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

void SceneECS::CleanScene()
{
    RemoveObjectsFromScene();
    ResetUniqueObjects();
}

void SceneECS::ResetUniqueObjects()
{
    //cursor.reset();
    //Renderer::controller.Camera->Reset();
}

void SceneECS::LoadSceneFromFile(const QString &filename)
{
    CleanScene();

    MG1::SceneSerializer ser;
    ser.LoadScene(filename.toStdString());
    MG1::Scene& scene = MG1::Scene::Get();

    LoadHelper<Point, MG1::Point>(scene.points);
    LoadHelper<Torus, MG1::Torus>(scene.tori);
    LoadHelper<BezierC0, MG1::BezierC0>(scene.bezierC0);
    LoadHelper<BezierC2, MG1::BezierC2>(scene.bezierC2);
    LoadHelper<InterpolationC2, MG1::InterpolatedC2>(scene.interpolatedC2);
    LoadHelper<PlaneC0, MG1::BezierSurfaceC0>(scene.surfacesC0);
    LoadHelper<PlaneC2, MG1::BezierSurfaceC2>(scene.surfacesC2);
}

void SceneECS::SaveSceneToFile(const QString &filename)
{
    if (auto sceneElements = GetSystem<SceneElementSystem>().lock())
    {
        sceneElements->SerializeSceneObjects();
        MG1::SceneSerializer ser;
        ser.SaveScene(filename.toStdString());
    }
}

void SceneECS::UpdateObjectId(uint oid, uint new_oid)
{
    for (auto s: systems)
    {
        s.second->UpdateObjectId(oid, new_oid);
    }
}




