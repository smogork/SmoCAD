//
// Created by ksm on 3/22/22.
//

#ifndef SMOCAD_SCENEMODEL_H
#define SMOCAD_SCENEMODEL_H


#include "Objects/CursorObject.h"

class SceneModel
{
private:
    std::list<IRenderableObject*> renderableObjects;
    std::unique_ptr<CursorObject> cursor = nullptr;
    std::list<IRenderableObject*> selectedObjects;

    void InitializeScene();

public:
    SceneModel();
    ~SceneModel();

    const std::list<IRenderableObject*>& GetRenderableObjects();
    const std::unique_ptr<CursorObject>& GetCursorObject();
    const std::list<IRenderableObject*>& GetSelectedObjects();

    void UpdateCursor(QVector3D position);
    void DeleteCursor();

    void AddObject(IRenderableObject* ro);
    void RemoveObject(IRenderableObject* ro);
    void ReleaseObjectsOnScene();

    void SelectObject(IRenderableObject* ro);
    void AppendToSelectedObjects(IRenderableObject* ro);
    void UnselectObjects();
};


#endif //SMOCAD_SCENEMODEL_H
