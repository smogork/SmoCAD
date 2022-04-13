//
// Created by ksm on 4/9/22.
//

#include "TransformCollection.h"
#include "Scene/SceneECS.h"
#include "Scene/Systems/TransformCollectionSystem.h"

std::shared_ptr<TransformCollection> TransformCollection::CreateRegisteredComponent(unsigned int oid)
{
    if (auto scene = SceneECS::Instance().lock())
    {
        auto system = scene->GetSystem<TransformCollectionSystem>().lock();
        return system->CreateRegistered(oid);
    }
    return nullptr;
}

void TransformCollection::UnregisterComponent()
{
    if (auto scene = SceneECS::Instance().lock())
    {
        auto system = scene->GetSystem<TransformCollectionSystem>().lock();
        system->Unregister(GetAttachedObjectID());
    }
}

TransformCollection::TransformCollection(unsigned int oid): IComponent(oid, TRANSFORM_COLLECTION)
{

}

TransformCollection::~TransformCollection()
{
    Clear();
    UnregisterComponent();
}

void TransformCollection::AddPoint(std::shared_ptr<CollectionAware> newObject)
{
    //[TODO] zastanowic sie nad wielokrotnymi punktami
    //std::list<std::weak_ptr<Transform>> toRemove;
    for (const std::weak_ptr<Transform>& el_weak : points)
    {
        if (auto el = el_weak.lock())
        {
            if (el->GetAttachedObjectID() == newObject->GetAttachedObjectID())
                return;
        }
        //else
            //toRemove.push_back(el_weak);
    }

    ConnectSignals(newObject->p_Transform);
    points.push_back(newObject->p_Transform);
    emit PointInCollectionModified();
}

//Podlacz sygnal zmiany polozenia do kolekcji
void TransformCollection::ConnectSignals(std::shared_ptr<Transform> p)
{
    std::weak_ptr<Transform> weakP = p;
    pointNotifiers.push_back(p->Position.addNotifier([this, weakP]()
    {
        if (auto p = weakP.lock())
        {
            emit this->SinglePointChanged(p->Position, p->GetAttachedObjectID());
        }
    }));
}

void TransformCollection::Clear()
{
    pointNotifiers.clear();
    points.clear();
    emit PointInCollectionModified();
}

void TransformCollection::CollectionElementTransformChanged(QVector3D val)
{
    emit PointInCollectionModified();
}

const std::list<std::weak_ptr<Transform>> &TransformCollection::GetPoints()
{
    //[TODO] wyeliminuj przy pobieraniu punktów te Transformy, które już straciły kontekst

    return points;
}

int TransformCollection::Size()
{
    return points.size();
}

/*const std::weak_ptr<Transform> &TransformCollection::operator[](std::size_t idx) const
{
    return points[idx];
}*/

TransformCollection &TransformCollection::operator=(const TransformCollection &other)
{
    pointNotifiers.clear();
    points.clear();

    for (const std::weak_ptr<Transform>& el_weak : other.points)
    {
        if (auto el = el_weak.lock())
        {
            points.push_back(el);
            ConnectSignals(el);
        }
    }

    emit PointInCollectionModified();
    return *this;
}
