//
// Created by ksm on 4/3/22.
//

#ifndef SMOCAD_SCENEECS_H
#define SMOCAD_SCENEECS_H


#include <QObject>
#include <QString>
#include <QListWidget>
#include "Scene/Systems/ISystem.h"
#include "Scene/Utilities/TypeMap.h"
#include "Scene/Entities/IEntity.h"
#include "Scene/Entities/Grid.h"
#include "Scene/Entities/Cursor.h"
#include "Scene/Entities/Composite.h"
#include "Scene/Systems/SceneElementSystem.h"
#include "Controls/ListElements/QListWidgetSceneElement.h"
#include "Scene/Entities/Mesh.h"
#include "Scene/Entities/Points/Point.h"

class SceneECS : public QObject
{
Q_OBJECT
public:
    static std::weak_ptr<SceneECS> Instance();

    static const unsigned int NON_OBJECT_ID = 0;

    ~SceneECS() override;

    unsigned int GetNewObjectID();

    template<typename S>
    std::weak_ptr<S> GetSystem()
    {
        auto it = systems.find<S>();
        assert(it != systems.end());
        return std::static_pointer_cast<S>(it->second);
    }

    template<typename S>
    bool IsObjectInSystem(unsigned int oid)
    {
        auto it = systems.find<S>();
        assert(it != systems.end());
        auto system = std::static_pointer_cast<S>(it->second);
        return system->IsObjectInSystem(oid);
    }

    //[TODO] trzeba przerobic aby przekazywac tylko system/component
    template<typename S, typename C>
    std::weak_ptr<C> GetComponentOfSystem(unsigned int oid)
    {
        auto it = systems.find<S>();
        assert(it != systems.end());
        auto system = std::static_pointer_cast<S>(it->second);
        return system->GetComponent(oid);
    }

    unsigned int MouseClicked(std::shared_ptr<SceneMouseClickEvent> event);

    void AddObject(std::shared_ptr<IEntity> obj);

    void AddObjectExplicitPosition(std::shared_ptr<IEntity> obj);

    void RemoveObject(unsigned int oid);

    std::list<std::unique_ptr<ComponentControl>> CreateUIForObject(unsigned int oid);

    std::list<std::pair<QString, std::function<void(QListWidgetSceneElement *item)> > >
    CreateContextMenuForSceneElement(unsigned int oid, int selectionCount);

    void RemoveObjectsFromScene();

    void RemoveUniqueObjects();

    void ClearSystems();

    QString DebugSystemReport();

    void InitializeScene();

signals:
    void CursorChange(std::shared_ptr<Cursor> cur);

private:
    static std::shared_ptr<SceneECS> scene;
    unsigned int objectCounter;
    TypeMap<std::shared_ptr<IAbstractSystem>> systems;
    std::list<std::shared_ptr<IEntity>> objects;

    std::unique_ptr<Grid> grid = nullptr;
    std::shared_ptr<Cursor> cursor = nullptr;
    std::unique_ptr<Composite> composite = nullptr;

    SceneECS();

    void InitUniqueObjects();
    void InitSceneObjects();

    void UpdateCursorObject(QVector3D cursorPos);
};


#endif //SMOCAD_SCENEECS_H
