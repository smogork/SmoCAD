//
// Created by ksm on 4/9/22.
//

#ifndef SMOCAD_TRANSFORMCOLLECTION_H
#define SMOCAD_TRANSFORMCOLLECTION_H

#include "IComponent.h"
#include "Transform.h"
#include "Scene/Components/Awares/CollectionAware.h"
#include <vector>

//Komponent przedstawiajacy zbior pewnych punktów (transformacji) w przestrzeni
//Jeden punkt moze znajdowac sie w wielu kolekcjach jednoczesnie
class TransformCollection: public IComponent
{
    Q_OBJECT
private:
    bool m_locked = false;
    std::list<std::weak_ptr<CollectionAware>> points;
    std::map<unsigned int, QPropertyNotifier> pointNotifiers;

    void ConnectSignals(std::shared_ptr<Transform> p);

private slots:
    void CollectionElementTransformChanged(QVector3D val);
    void PointFromCollectionHasBeenDeleted(unsigned int deletedOid);

public:
    int SecondDimension = 0;

    static std::shared_ptr<TransformCollection> CreateRegisteredComponent(unsigned int oid);
    void UnregisterComponent();

    explicit TransformCollection(unsigned int oid);
    ~TransformCollection() override;

    const std::list<std::weak_ptr<Transform>> GetPoints();
    std::vector<std::weak_ptr<CollectionAware>> GetVectorAwares();
    std::vector<QVector3D> GetVectorCoords();
    void SetPoints(std::vector<std::shared_ptr<CollectionAware>> newPoints);
    void AddPoint(std::shared_ptr<CollectionAware> newObject);
    void RemovePoint(unsigned int oid);
    void ReplaceObject(unsigned int toReplace, std::shared_ptr<CollectionAware> newElement);
    void Clear();
    int Size();

    bool IsContentLocked();
    void LockContent(bool state = true);

    //const std::weak_ptr<Transform>& operator[](std::size_t idx) const;
    TransformCollection& operator=(const TransformCollection& other);

signals:
    //Sygnal informuje o zmianie polozenia pewnego punktu w kolekcji
    void PointInCollectionModified();
    void SinglePointChanged(QVector3D position, unsigned int changedOID);
    void PointDeleted();
};


#endif //SMOCAD_TRANSFORMCOLLECTION_H
