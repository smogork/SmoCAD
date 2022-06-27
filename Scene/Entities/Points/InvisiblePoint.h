//
// Created by ksm on 4/22/22.
//

#ifndef SMOCAD_INVISIBLEPOINT_H
#define SMOCAD_INVISIBLEPOINT_H

#include "Scene/Entities/IEntity.h"
#include "Scene/Components/Awares/CollectionAware.h"

class InvisiblePoint: public IEntity
{
public:
    std::shared_ptr<Transform> p_Transform;
    std::shared_ptr<CollectionAware> p_CollectionAware;

    explicit InvisiblePoint(QVector3D pos);
};

#endif //SMOCAD_INVISIBLEPOINT_H
