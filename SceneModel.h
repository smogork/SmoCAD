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

    void InitializeScene();

public:
    SceneModel();
    ~SceneModel();

    std::shared_ptr<CursorObject> Cursor = nullptr;

    const std::list<IRenderableObject*>& GetRenderableObjects();

    void AddObject(IRenderableObject* ro);
    void ReleaseObjectsOnScene();
};


#endif //SMOCAD_SCENEMODEL_H
