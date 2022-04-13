//
// Created by ksm on 4/9/22.
//

#ifndef SMOCAD_TRANSFORMCOLLECTION_H
#define SMOCAD_TRANSFORMCOLLECTION_H

#include "IComponent.h"
#include "Transform.h"
#include "CollectionAware.h"
#include <vector>

//Komponent przedstawiajacy zbior pewnych punktów (transformacji) w przestrzeni
//Jeden punkt moze znajdowac sie w wielu kolekcjach jednoczesnie
class TransformCollection: public IComponent
{
    Q_OBJECT
private:
    std::list<std::weak_ptr<Transform>> points;
    std::list<QPropertyNotifier> pointNotifiers;

    void ConnectSignals(std::shared_ptr<Transform> p);

private slots:
    void CollectionElementTransformChanged(QVector3D val);

public:
    static std::shared_ptr<TransformCollection> CreateRegisteredComponent(unsigned int oid);
    void UnregisterComponent();

    explicit TransformCollection(unsigned int oid);
    ~TransformCollection() override;

    const std::list<std::weak_ptr<Transform>>& GetPoints();
    void AddPoint(std::shared_ptr<CollectionAware> newObject);
    void Clear();
    int Size();

    //const std::weak_ptr<Transform>& operator[](std::size_t idx) const;
    TransformCollection& operator=(const TransformCollection& other);

signals:
    //Sygnal informuje o zmianie polozenia pewnego punktu w kolekcji
    void PointInCollectionModified();
    void SinglePointChanged(QVector3D position, unsigned int changedOID);
};


#endif //SMOCAD_TRANSFORMCOLLECTION_H
