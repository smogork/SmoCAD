//
// Created by ksm on 4/9/22.
//

#ifndef SMOCAD_TRANSFORMCOLLECTION_H
#define SMOCAD_TRANSFORMCOLLECTION_H

#include "IComponent.h"
#include "Transform.h"
#include "CollectionAware.h"

//Komponent przedstawiajacy zbior pewnych punktów (transformacji) w przestrzeni
//Jeden punkt moze znajdowac sie w wielu kolekcjach jednoczesnie
class TransformCollection: public IComponent
{
private:


public:
    std::list<std::shared_ptr<Transform>> Points;

    static std::shared_ptr<TransformCollection> CreateRegisteredComponent(unsigned int oid);
    void UnregisterComponent();

    explicit TransformCollection(unsigned int oid);
    ~TransformCollection() override;

    void AddPoint(std::shared_ptr<CollectionAware> newObject);
    /*void Delete(unsigned int oid);
    void Clear();*/

signals:
    void PointInCollectionModified();
};


#endif //SMOCAD_TRANSFORMCOLLECTION_H
