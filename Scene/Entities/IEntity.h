//
// Created by ksm on 4/3/22.
//

#ifndef SMOCAD_IENTITY_H
#define SMOCAD_IENTITY_H

#include <QObject>
#include "Scene/Utilities/TypeMap.h"
#include "Scene/Components/IComponent.h"

enum ENTITY_CLASS
{
    POINT_CLASS,
    GRID_CLASS,
    CUBE_CLASS,
    TORUS_CLASS,
    CURSOR_CLASS,
    COMPOSITE_CLASS,
    COMPOSITE_ELEM_CLASS,
    POLYLINE_CLASS,
    BEZIERC0_CLASS,
    BEZIERC2_CLASS,
    VIRTUAL_POINT_CLASS,
    CLASS_COUNT
};

class IEntity: public QObject
{
    Q_OBJECT
private:
    TypeMap<std::shared_ptr<IComponent>> m_components;

protected:
    unsigned int objectID;
    unsigned int classID;

    template <typename C>
    void AddComponent(std::shared_ptr<C> component)
    {
        m_components.put<C>(std::static_pointer_cast<IComponent>(component));
    }

public:

    explicit IEntity(unsigned int cid);
    virtual ~IEntity() override;

    unsigned int GetObjectID();
    unsigned int GetClassID();

    template <typename C>
    std::weak_ptr<C> GetComponent()
    {
        auto it = m_components.find<C>();

        if (it == m_components.end())
            return std::shared_ptr<C>();
        return std::static_pointer_cast<C>(it->second);
    }
};


#endif //SMOCAD_IENTITY_H
