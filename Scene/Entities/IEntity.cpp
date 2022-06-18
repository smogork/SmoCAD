//
// Created by ksm on 4/3/22.
//

#include "IEntity.h"
#include "Scene/SceneECS.h"
#include <QString>

const QColor IEntity::DefaultColor = QColor::fromRgbF(0.8f, 0.8f, 0.8f, 1.0f);

IEntity::IEntity(unsigned int cid)
{
    if (cid >= ENTITY_CLASS::CLASS_COUNT)
        throw std::runtime_error(QString("entity classID %1 unknown").arg(cid).toStdString());

    classID = cid;
    if (auto scene = SceneECS::Instance().lock())
        m_objectId = scene->GetNewObjectID();
}

unsigned int IEntity::GetObjectID()
{
    return m_objectId;
}

unsigned int IEntity::GetClassID()
{
    return classID;
}

IEntity::~IEntity()
{
    m_deleted = true;
    emit EntityDeleted(m_objectId);
}

void IEntity::SetObjectId(uint oid)
{
    uint old_oid = m_objectId;
    m_objectId = oid;

    if (auto scene = SceneECS::Instance().lock())
        scene->UpdateObjectId(old_oid, oid);
}

IEntity::IEntity(unsigned int cid, unsigned int explicit_oid)
{
    if (cid >= ENTITY_CLASS::CLASS_COUNT)
        throw std::runtime_error(QString("entity classID %1 unknown").arg(cid).toStdString());

    classID = cid;
    m_objectId = explicit_oid;
    if (auto scene = SceneECS::Instance().lock())
        scene->SetMaxOID(explicit_oid);
}
