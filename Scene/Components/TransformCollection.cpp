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

TransformCollection::TransformCollection(unsigned int oid) : IComponent(oid, TRANSFORM_COLLECTION)
{ }

TransformCollection::~TransformCollection()
{
    pointNotifiers.clear();
    points.clear();
    UnregisterComponent();
}

void TransformCollection::AddPoint(std::shared_ptr<CollectionAware> newObject)
{
    ConnectSignals(newObject->p_Transform);
    points.push_back(newObject->p_Transform);
    emit PointInCollectionModified();
}

//Podlacz sygnal zmiany polozenia do kolekcji
void TransformCollection::ConnectSignals(std::shared_ptr<Transform> p)
{
    if (pointNotifiers.contains(p->GetAttachedObjectID()))
        return;

    std::weak_ptr<Transform> weakP = p;
    pointNotifiers.insert(std::make_pair(p->GetAttachedObjectID(),
    p->Position.addNotifier([this, weakP]()
    {
       if (auto p = weakP.lock())
       {
           emit this->SinglePointChanged(
                   p->Position,
                   p->GetAttachedObjectID());
       }
    })));

    connect(p.get(), &IComponent::ComponentDeleted, this, &TransformCollection::PointFromCollectionHasBeenDeleted);
}

void TransformCollection::Clear()
{
    pointNotifiers.clear();
    points.clear();
    //disconnect(nullptr, &IComponent::ComponentDeleted, this, &TransformCollection::PointFromCollectionHasBeenDeleted);
    emit PointInCollectionModified();
}

void TransformCollection::CollectionElementTransformChanged(QVector3D val)
{
    emit PointInCollectionModified();
}

const std::list<std::weak_ptr<Transform>> &TransformCollection::GetPoints()
{
    return points;
}

int TransformCollection::Size()
{
    return points.size();
}

TransformCollection &TransformCollection::operator=(const TransformCollection &other)
{
    pointNotifiers.clear();
    points.clear();
    //disconnect(nullptr, &IComponent::ComponentDeleted, this, &TransformCollection::PointFromCollectionHasBeenDeleted);

    for (const std::weak_ptr<Transform> &el_weak: other.points)
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

void TransformCollection::RemovePoint(unsigned int oid)
{
    auto weak_item = std::find_if(points.begin(), points.end(),[oid](std::weak_ptr<Transform> wp)
    {
        if (auto p = wp.lock())
            return p->GetAttachedObjectID() == oid;
        return false;
    });
    if (auto item = (*weak_item).lock())
        disconnect(item.get(), &IComponent::ComponentDeleted, this, &TransformCollection::PointFromCollectionHasBeenDeleted);

    points.remove_if([oid](std::weak_ptr<Transform> wp)
                     {
                         if (auto p = wp.lock())
                             return p->GetAttachedObjectID() == oid;
                         return true;
                     });
    pointNotifiers.erase(oid);

    emit PointInCollectionModified();
}

void TransformCollection::PointFromCollectionHasBeenDeleted(unsigned int deletedOid)
{
    auto oid = deletedOid;
    //RemovePoint(deletedOid);
    //emit PointInCollectionModified();
    /*auto weak_item = std::find_if(points.begin(), points.end(),[oid](std::weak_ptr<Transform> wp)
    {
        if (auto p = wp.lock())
            return p->GetAttachedObjectID() == oid;
        return false;
    });*/
    /*if (auto item = (*weak_item).lock())
        disconnect(item.get(), &IComponent::ComponentDeleted, this, &TransformCollection::PointFromCollectionHasBeenDeleted);*/

    points.remove_if([oid](std::weak_ptr<Transform> wp)
                     {
                         if (auto p = wp.lock())
                             return p->GetAttachedObjectID() == oid;
                         return true;
                     });
    pointNotifiers.erase(oid);
    emit PointInCollectionModified();
}

void TransformCollection::SetPoints(std::vector<std::shared_ptr<CollectionAware>> newPoints)
{
    pointNotifiers.clear();
    points.clear();

    for (const std::shared_ptr<CollectionAware> &el: newPoints)
    {
        points.push_back(el->p_Transform);
        ConnectSignals(el->p_Transform);
    }

    emit PointInCollectionModified();
}
