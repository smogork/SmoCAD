//
// Created by ksm on 3/22/22.
//

#include <cfloat>
#include "SceneModelOld.h"
#include "Objects/CubeObject.h"
#include "Objects/PointObject.h"
#include "Objects/TorusObject.h"
#include "Scene/Events/PointObjectChangedEvent.h"

void SceneModelOld::InitializeScene()
{
    //Tutaj poczatkowe ohbiektu dodac
    renderableObjects.push_back(new CubeObject(QVector3D(0.0f, 5.0f, 5.0f)));
    renderableObjects.push_back(new CubeObject(QVector3D(0.0f, 0.0f, 5.0f)));
    renderableObjects.push_back(new TorusObject(QVector3D(5.0f, 0.0f, 10.0f), 5, 1, 36, 18));
    renderableObjects.push_back(new PointObject(QVector3D(0.0f, 0.0f, 0.0f)));//3
    renderableObjects.push_back(new PointObject(QVector3D(1.0f, 0.0f, 0.0f)));
    renderableObjects.push_back(new PointObject(QVector3D(2.0f, 0.0f, 1.0f)));
    renderableObjects.push_back(new PointObject(QVector3D(2.0f, 1.0f, 1.0f)));
    renderableObjects.push_back(new PointObject(QVector3D(2.0f, 1.0f, -1.0f)));
    renderableObjects.push_back(new PointObject(QVector3D(0.0f, -1.0f, -1.0f)));
    renderableObjects.push_back(new PointObject(QVector3D(-2.0f, 0.0f, -0.0f)));
    renderableObjects.push_back(new PointObject(QVector3D(-5.0f, 0.0f, -1.0f)));
}

SceneModelOld::~SceneModelOld()
{

}

SceneModelOld::SceneModelOld() { InitializeScene(); }

const std::list<IRenderableObject *> &SceneModelOld::GetRenderableObjects()
{
    return renderableObjects;
}

bool SceneModelOld::AddObject(IRenderableObject *ro, bool positionless)
{
    if (ro)
    {
        if (!cursor && !positionless)
            return false;

        if (cursor && !positionless)
            ro->Position = cursor->Position;

        renderableObjects.push_back(ro);
        return true;
    }
    return false;
}

void SceneModelOld::ReleaseObjectsOnScene()
{
    for (IRenderableObject* ro : renderableObjects)
        delete ro;
    renderableObjects.clear();
}

void SceneModelOld::UpdateCursor(QVector3D position)
{
    if (cursor)
        cursor->Position = position;
    else
        cursor = std::make_unique<CursorObject>(position);
}

void SceneModelOld::DeleteCursor()
{
   cursor.reset();
}

const std::unique_ptr<CursorObject> &SceneModelOld::GetCursorObject()
{
    return cursor;
}

bool SceneModelOld::SelectObject(IRenderableObject *ro)
{
    if (!ro)
        return false;

    UnselectObjects();
    selectedObject = ro;
    selectedObject->Selected = true;

    auto event =std::make_shared<SelectedObjectChangedEvent>(selectedObject);
    emit SelectedObjectChanged(event);
    return true;
}

IRenderableObject* SceneModelOld::GetSelectedObject()
{
    return selectedObject;
}

void SceneModelOld::RemoveObject(IRenderableObject *ro)
{
    if (ro)
    {
        renderableObjects.remove(ro);
        delete ro;
    }
}

bool SceneModelOld::AppendToSelectedObjects(IRenderableObject *ro)
{
    if (!ro || dynamic_cast<BezierCurveC0*>(ro))
        return false;

    if (selectedObject && !composite)
    {
        if (dynamic_cast<BezierCurveC0*>(selectedObject) || ro == selectedObject)
            return false;

        composite = std::make_unique<CompositeObject>(selectedObject, ro);
        renderableObjects.remove(selectedObject);
        renderableObjects.remove(ro);
        selectedObject = nullptr;
    }
    else
    {
        composite->AddObject(ro);
        renderableObjects.remove(ro);
    }

    auto event =std::make_shared<SelectedObjectChangedEvent>(composite.get());
    emit SelectedObjectChanged(event);
    return true;
}

void SceneModelOld::UnselectObjects()
{
    if (composite)
    {
        composite->ApplyTransformationsToChildren();
        for (CompositeObject::CompositeTransform &o: composite->GetObjects())
            renderableObjects.push_back(o.Object);
        composite.reset();
    }
    if (selectedObject)
    {
        selectedObject->Selected = false;
        selectedObject = nullptr;
    }
}

const std::unique_ptr<CompositeObject> &SceneModelOld::GetCompositeObject()
{
    return composite;
}

void SceneModelOld::RemoveComposite()
{
    if (composite)
    {
        for (CompositeObject::CompositeTransform &o: composite->GetObjects())
            delete o.Object;
        composite.reset();
    }
}

bool SceneModelOld::SelectObjectByMouse(QVector4D raycastStart, QVector4D raycastDirection)
{
    //UnselectObjects();

    float t_min = FLT_MAX;
    IRenderableObject* closest = nullptr;
    for (IRenderableObject* ro : renderableObjects)
    {
        float t = ro->TestAgainstRaycast(raycastStart, raycastDirection);
        if (t != NAN && t < t_min)
        {
            t_min = t;
            closest = ro;
        }
    }

    if (closest)
        SelectObject(closest);

    return closest != nullptr;
}
