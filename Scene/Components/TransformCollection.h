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
    Q_OBJECT
private:
    std::list<std::shared_ptr<Transform>> points;
    std::list<QPropertyNotifier> pointNotifiers;

    void ConnectSignals(std::shared_ptr<Transform> p);
    void DisconnectSignals(std::shared_ptr<Transform> p);

private slots:
    void CollectionElementTransformChanged(QVector3D val);

public:
    static std::shared_ptr<TransformCollection> CreateRegisteredComponent(unsigned int oid);
    void UnregisterComponent();

    explicit TransformCollection(unsigned int oid);
    ~TransformCollection() override;

    const std::list<std::shared_ptr<Transform>>& GetPoints();
    void AddPoint(std::shared_ptr<CollectionAware> newObject);
    //void Delete(unsigned int oid);
    void Clear();
    int Size();

signals:
    //Sygnal informuje o zmianie polozenia pewnego punktu w kolekcji
    void PointInCollectionModified();
};


#endif //SMOCAD_TRANSFORMCOLLECTION_H
