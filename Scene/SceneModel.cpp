//
// Created by ksm on 3/22/22.
//

#include "SceneModel.h"
#include "Objects/CubeObject.h"
#include "Objects/PointObject.h"
#include "Objects/TorusObject.h"

void SceneModel::InitializeScene()
{
    //Tutaj poczatkowe ohbiektu dodac
    renderableObjects.push_back(new CubeObject(QVector3D(0.0f, 5.0f, 5.0f)));
    renderableObjects.push_back(new CubeObject(QVector3D(0.0f, 0.0f, 5.0f)));
    renderableObjects.push_back(new PointObject(QVector3D(0.0f, 0.0f, 0.0f)));
    renderableObjects.push_back(new PointObject(QVector3D(1.0f, 0.0f, 0.0f)));
    renderableObjects.push_back(new PointObject(QVector3D(2.0f, 0.0f, 1.0f)));
    renderableObjects.push_back(new TorusObject(QVector3D(5.0f, 0.0f, 10.0f), 5, 1, 36, 18));
}

SceneModel::~SceneModel()
{

}

SceneModel::SceneModel() { InitializeScene(); }

const std::list<IRenderableObject *> &SceneModel::GetRenderableObjects()
{
    return renderableObjects;
}

void SceneModel::AddObject(IRenderableObject *ro)
{
    if (ro && cursor)
    {
        ro->Position = cursor->Position;
        renderableObjects.push_back(ro);
    }
}

void SceneModel::ReleaseObjectsOnScene()
{
    for (IRenderableObject* ro : renderableObjects)
        delete ro;
    renderableObjects.clear();
}

void SceneModel::UpdateCursor(QVector3D position)
{
    if (cursor)
        cursor->Position = position;
    else
        cursor = std::make_unique<CursorObject>(position);
}

void SceneModel::DeleteCursor()
{
   cursor.reset();
}

const std::unique_ptr<CursorObject> &SceneModel::GetCursorObject()
{
    return cursor;
}

void SceneModel::SelectObject(IRenderableObject *ro)
{
    if (!ro)
        return;

    UnselectObjects();
    selectedObject = ro;
    selectedObject->Selected = true;

    auto event =std::make_shared<SelectedObjectChangedEvent>(selectedObject);
    emit SelectedObjectChanged(event);
}

IRenderableObject* SceneModel::GetSelectedObject()
{
    return selectedObject;
}

void SceneModel::RemoveObject(IRenderableObject *ro)
{
    if (ro)
    {
        renderableObjects.remove(ro);
        delete ro;
    }
}

void SceneModel::AppendToSelectedObjects(IRenderableObject *ro)
{
    if (!ro)
        return;

    if (selectedObject && !composite)
    {
        composite = std::make_unique<CompositeObject>(selectedObject, ro);
        selectedObject = nullptr;
    }
    else
    {
        composite->AddObject(ro);
    }

    auto event =std::make_shared<SelectedObjectChangedEvent>(composite.get());
    emit SelectedObjectChanged(event);
}

void SceneModel::UnselectObjects()
{
    if (composite)
    {
        composite->ApplyTransformationsToChildren();
        composite.reset();
    }
    if (selectedObject)
    {
        selectedObject->Selected = false;
    }
}

const std::unique_ptr<CompositeObject> &SceneModel::GetCompositeObject()
{
    return composite;
}

void SceneModel::RemoveComposite()
{
    if (composite)
    {
        for (CompositeObject::CompositeTransform &o: composite->GetObjects())
            delete o.Object;
        composite.reset();
    }
}
