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
    for (const std::shared_ptr<Transform>& el : points)
        if (el->GetAttachedObjectID() == newObject->GetAttachedObjectID())
            return;

    ConnectSignals(newObject->p_Transform);
    points.push_back(newObject->p_Transform);
    emit PointInCollectionModified();
}

//Podlacz sygnal zmiany polozenia do kolekcji
void TransformCollection::ConnectSignals(std::shared_ptr<Transform> p)
{
    pointNotifiers.push_back(p->Position.addNotifier([this]()
    {
        emit this->PointInCollectionModified();
    }));
}

//Odlacz wszystkie sygnaly od kolekcji
void TransformCollection::DisconnectSignals(std::shared_ptr<Transform> p)
{
    //p->disconnect(this);
}

/*void TransformCollection::Delete(unsigned int oid)
{
    bool found = false;
    for (const std::shared_ptr<Transform>& el : points)
        if (el->GetAttachedObjectID() == oid)
        {
            DisconnectSignals(el);
            found = true;
            break;
        }

    if (!found)
        return;

    points.remove_if([oid](std::shared_ptr<Transform>& el) {
       return el->GetAttachedObjectID() == oid;
    } );
    emit PointInCollectionModified();
}*/

void TransformCollection::Clear()
{
    /*for (const std::shared_ptr<Transform>& el : points)
            DisconnectSignals(el);*/

    pointNotifiers.clear();
    points.clear();
    emit PointInCollectionModified();
}

void TransformCollection::CollectionElementTransformChanged(QVector3D val)
{
    emit PointInCollectionModified();
}

const std::list<std::shared_ptr<Transform>> &TransformCollection::GetPoints()
{
    return points;
}

int TransformCollection::Size()
{
    return points.size();
}
