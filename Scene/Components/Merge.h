//
// Created by ksm on 6/12/22.
//

#ifndef SMOCAD_MERGE_H
#define SMOCAD_MERGE_H

#include "IComponent.h"
#include "CollectionAware.h"

class Merge : public IComponent
{
public:
    std::shared_ptr<CollectionAware> p_CollectionAware;
    std::shared_ptr<Transform> p_Transform;
    
    static std::shared_ptr<Merge>
    CreateRegisteredComponent(unsigned int oid, std::shared_ptr<Transform> trans, std::shared_ptr<CollectionAware> col = nullptr);
    void UnregisterComponent();
    
    explicit Merge(unsigned int oid);
    ~Merge() override;
};

#endif //SMOCAD_MERGE_H
