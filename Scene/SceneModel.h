//
// Created by ksm on 3/22/22.
//

#ifndef SMOCAD_SCENEMODEL_H
#define SMOCAD_SCENEMODEL_H

#include <QObject>
#include "Objects/CursorObject.h"
#include "Scene/Events/SelectedObjectChangedEvent.h"
#include "Objects/CompositeObject.h"

class SceneModel: public QObject
{
    Q_OBJECT

private:
    std::list<IRenderableObject*> renderableObjects;
    std::unique_ptr<CursorObject> cursor = nullptr;
    IRenderableObject* selectedObject = nullptr;
    std::unique_ptr<CompositeObject> composite = nullptr;

    void InitializeScene();

public:
    SceneModel();
    ~SceneModel();

    const std::list<IRenderableObject*>& GetRenderableObjects();
    const std::unique_ptr<CursorObject>& GetCursorObject();
    const std::unique_ptr<CompositeObject>& GetCompositeObject();
    IRenderableObject* GetSelectedObject();

    void UpdateCursor(QVector3D position);
    void DeleteCursor();

    void AddObject(IRenderableObject* ro);
    void RemoveObject(IRenderableObject* ro);
    void RemoveComposite();
    void ReleaseObjectsOnScene();

    void SelectObject(IRenderableObject* ro);
    void AppendToSelectedObjects(IRenderableObject* ro);
    void UnselectObjects();

signals:
    void SelectedObjectChanged(std::shared_ptr<SelectedObjectChangedEvent> event);

};


#endif //SMOCAD_SCENEMODEL_H
