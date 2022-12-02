//
// Created by ksm on 12/1/22.
//

#ifndef SMOCAD_OFFSETPLANE_H
#define SMOCAD_OFFSETPLANE_H

#include "Scene/Entities/IEntity.h"
#include "Scene/Components/UVParams.h"
#include "Scene/Components/Awares/IntersectionAware.h"

class OffsetPlane: public IEntity
{
private:
    static constexpr float h = 5e-4;

    std::weak_ptr<IntersectionAware> m_innerPlane;
    float m_offset;

    QVector3D OffsetPlaneFunc(QVector2D uv);
    QVector3D OffsetPlaneFuncDerU(QVector2D uv);
    QVector3D OffsetPlaneFuncDerV(QVector2D uv);
    void InitializeUV(std::shared_ptr<IntersectionAware> inner);

public:
    std::shared_ptr<UVParams> p_UV;
    std::shared_ptr<IntersectionAware> p_Intersection;

    OffsetPlane(std::shared_ptr<IntersectionAware> innerPlane, float offset);
};

#endif //SMOCAD_OFFSETPLANE_H
