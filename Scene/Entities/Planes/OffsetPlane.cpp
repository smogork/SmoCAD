//
// Created by ksm on 12/1/22.
//

#include "OffsetPlane.h"

OffsetPlane::OffsetPlane(std::shared_ptr<IntersectionAware> innerPlane, float offset): IEntity(OFFSET_PLANE)
{
    m_innerPlane = innerPlane;
    m_offset = offset;
    AddComponent(p_UV = UVParams::CreateRegisteredComponent(GetObjectID(), innerPlane->p_UV->U, innerPlane->p_UV->V));
    AddComponent(p_Intersection = IntersectionAware::CreateRegisteredComponent(GetObjectID(), p_UV));
    InitializeUV(innerPlane);
}

void OffsetPlane::InitializeUV(std::shared_ptr<IntersectionAware> inner)
{
    p_Intersection->SceneFunction = ASSIGN_UV_FUNCTION(&OffsetPlane::OffsetPlaneFunc);
    p_Intersection->SceneFunctionDerU = ASSIGN_UV_FUNCTION(&OffsetPlane::OffsetPlaneFuncDerU);
    p_Intersection->SceneFunctionDerV = ASSIGN_UV_FUNCTION(&OffsetPlane::OffsetPlaneFuncDerV);


    p_Intersection->UWraps = *inner->UWraps;
    p_Intersection->VWraps = *inner->VWraps;
    p_Intersection->UMin = *inner->UMin;
    p_Intersection->VMin = *inner->VMin;
    p_Intersection->UMax = *inner->UMax;
    p_Intersection->VMax = *inner->VMax;
}

QVector3D OffsetPlane::OffsetPlaneFunc(QVector2D uv)
{
    if (auto inner = m_innerPlane.lock())
    {
        QVector3D N = QVector3D::crossProduct(inner->SceneFunctionDerU(uv), inner->SceneFunctionDerV(uv)).normalized();
        return inner->SceneFunction(uv) + m_offset * N;
    }
    return {NULL, NULL, NULL};
}

QVector3D OffsetPlane::OffsetPlaneFuncDerU(QVector2D uv)
{
    if (!std::isnan(m_hardcodedDerU.x()))
        return m_hardcodedDerU;

    return (OffsetPlaneFunc({uv.x() + h, uv.y()}) - OffsetPlaneFunc({uv.x() - h, uv.y()})) / (2 * h);
}

QVector3D OffsetPlane::OffsetPlaneFuncDerV(QVector2D uv)
{
    if (!std::isnan(m_hardcodedDerV.x()))
        return m_hardcodedDerV;

    return (OffsetPlaneFunc({uv.x(), uv.y() + h}) - OffsetPlaneFunc({uv.x(), uv.y() - h})) / (2 * h);
}

void OffsetPlane::SetHardcodedDerU(QVector3D derU)
{
    m_hardcodedDerU = derU;
}

void OffsetPlane::SetHardcodedDerV(QVector3D derV)
{
    m_hardcodedDerV = derV;
}

void OffsetPlane::ResetHardcodedDerU()
{
    m_hardcodedDerU = {NAN, NAN, NAN};
}

void OffsetPlane::ResetHardcodedDerV()
{
    m_hardcodedDerV = {NAN, NAN, NAN};
}
