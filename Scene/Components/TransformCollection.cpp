//
// Created by ksm on 4/9/22.
//

#include "TransformCollection.h"
#include "Scene/SceneECS.h"
#include "Scene/Systems/TransformCollectionSystem.h"

#include <ranges>

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

TransformCollection::TransformCollection(unsigned int oid) : IComponent(oid, TRANSFORM_COLLECTION) {}

TransformCollection::~TransformCollection()
{
    pointNotifiers.clear();
    points.clear();
    UnregisterComponent();
}

void TransformCollection::AddPoint(std::shared_ptr<CollectionAware> newObject)
{
    ConnectSignals(newObject->p_Transform);
    points.push_back(newObject);
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
    if (points.size() == 0)
        return;
    
    pointNotifiers.clear();
    points.clear();
    //disconnect(nullptr, &IComponent::ComponentDeleted, this, &TransformCollection::PointFromCollectionHasBeenDeleted);
    emit PointInCollectionModified();
}

void TransformCollection::CollectionElementTransformChanged(QVector3D val)
{
    emit PointInCollectionModified();
}

const std::list<std::weak_ptr<Transform>> TransformCollection::GetPoints()
{
    std::list<std::weak_ptr<Transform>> res;
    
    for (const auto &el: points)
        res.push_back(el.lock()->p_Transform);
    
    return res;
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
    
    for (const std::weak_ptr<CollectionAware> &el_weak: other.points)
    {
        if (auto el = el_weak.lock())
        {
            points.push_back(el);
            ConnectSignals(el->p_Transform);
        }
    }
    
    emit PointInCollectionModified();
    return *this;
}

void TransformCollection::RemovePoint(unsigned int oid)
{
    auto weak_item = std::find_if(points.begin(), points.end(), [oid](std::weak_ptr<CollectionAware> wp)
    {
        if (auto p = wp.lock())
            return p->GetAttachedObjectID() == oid;
        return false;
    });
    if (auto item = (*weak_item).lock())
        disconnect(item.get(), &IComponent::ComponentDeleted, this,
                   &TransformCollection::PointFromCollectionHasBeenDeleted);
    
    points.remove_if([oid](std::weak_ptr<CollectionAware> wp)
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
    
    points.remove_if([oid](std::weak_ptr<CollectionAware> wp)
                     {
                         if (auto p = wp.lock())
                             return p->GetAttachedObjectID() == oid;
                         return true;
                     });
    pointNotifiers.erase(oid);
    emit PointInCollectionModified();
    emit PointDeleted();
}

void TransformCollection::SetPoints(std::vector<std::shared_ptr<CollectionAware>> newPoints)
{
    pointNotifiers.clear();
    points.clear();
    
    for (const std::shared_ptr<CollectionAware> &el: newPoints)
    {
        points.push_back(el);
        ConnectSignals(el->p_Transform);
    }
    
    emit PointInCollectionModified();
}

void TransformCollection::LockContent(bool state)
{
    m_locked = state;
}

bool TransformCollection::IsContentLocked()
{
    return m_locked;
}

void TransformCollection::ReplaceObject(unsigned int toReplace, std::shared_ptr<CollectionAware> newElement)
{
    if (points.empty())
        return;
    
    auto weak_item = std::find_if(points.begin(), points.end(), [toReplace](std::weak_ptr<CollectionAware> wp)
    {
        if (auto p = wp.lock())
            return p->GetAttachedObjectID() == toReplace;
        return false;
    });
    if (weak_item == points.end())
        return;
    
    auto item = weak_item->lock();
    if (!item)
        return;
    
    //Usuniecie sygnalu o osunieciu obiektu ze sceny
    disconnect(item->p_Transform.get(), &IComponent::ComponentDeleted, this,
               &TransformCollection::PointFromCollectionHasBeenDeleted);
    pointNotifiers.erase(toReplace);
    
    //podmiana polozenia w liscie punktow
    std::replace_if(points.begin(), points.end(),
                    [toReplace](std::weak_ptr<CollectionAware> wp)
                    {
                        if (auto p = wp.lock())
                            return p->GetAttachedObjectID() == toReplace;
                        return false;
                    },
                    newElement
    );
    
    //Podlaczenie sygnalow dla nowo dodawanego obiektu
    ConnectSignals(newElement->p_Transform);
}

std::vector<std::weak_ptr<CollectionAware>> TransformCollection::GetVectorAwares()
{
    return std::vector<std::weak_ptr<CollectionAware>>(points.begin(), points.end());
}

std::vector<QVector3D> TransformCollection::GetVectorCoords()
{
    std::vector<QVector3D> res(points.size());
    
    int counter = 0;
    for (const auto& el : points)
        res[counter++] = el.lock()->p_Transform->Position.value();
    
    return res;
}
