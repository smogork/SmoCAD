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
    IRenderableObject* selectedObject = nullptr;

    void InitializeScene();

public:
    SceneModel();
    ~SceneModel();

    const std::list<IRenderableObject*>& GetRenderableObjects();
    const std::unique_ptr<CursorObject>& GetCursorObject();
    IRenderableObject* GetSelectedObject();

    void UpdateCursor(QVector3D position);
    void DeleteCursor();

    void AddObject(IRenderableObject* ro);
    void RemoveObject(IRenderableObject* ro);
    void ReleaseObjectsOnScene();

    void SelectObject(IRenderableObject* ro);
};


#endif //SMOCAD_SCENEMODEL_H
