//
// Created by ksm on 4/3/22.
//

#ifndef SMOCAD_SCENEECS_H
#define SMOCAD_SCENEECS_H


#include <QObject>
#include "Scene/Systems/ISystem.h"
#include "Scene/Utilities/TypeMap.h"
#include "Scene/Entities/IEntity.h"

class SceneECS: QObject
{
    Q_OBJECT
public:
    static std::weak_ptr<SceneECS> Instance();

    ~SceneECS() override;
    unsigned int GetNewObjectID();

    template <typename S>
    std::weak_ptr<S> GetSystem()
    {
        auto it = systems.find<S>();
        assert(it != systems.end());
        return std::static_pointer_cast<S>(it->second);
    }

    template <typename S>
    std::weak_ptr<S> GetComponentOfSystem(unsigned int oid)
    {
        auto it = systems.find<S>();
        assert(it != systems.end());
        auto system = std::static_pointer_cast<S>(it->second);
        return system->GetComponent(oid);
    }

private:
    static std::shared_ptr<SceneECS> scene;
    unsigned int objectCounter;
    TypeMap<std::shared_ptr<IAbstractSystem>> systems;
    std::list<std::shared_ptr<IEntity>> objects;

    SceneECS();
    void InitObjects();
};


#endif //SMOCAD_SCENEECS_H
