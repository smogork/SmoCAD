//
// Created by ksm on 4/9/22.
//

#ifndef SMOCAD_COLLECTIONAWARE_H
#define SMOCAD_COLLECTIONAWARE_H


#include "IComponent.h"
#include "Transform.h"

class CollectionAware: public IComponent
{
public:
    std::shared_ptr<Transform> p_Transform;

    static std::shared_ptr<CollectionAware> CreateRegisteredComponent(unsigned int oid, std::shared_ptr<Transform> t);
    void UnregisterComponent();

    explicit CollectionAware(unsigned int oid);
    ~CollectionAware() override;
};

#endif SMOCAD_COLLECTIONAWARE_H
