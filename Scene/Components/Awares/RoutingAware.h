//
// Created by ksm on 11/9/22.
//

#ifndef SMOCAD_ROUTINGAWARE_H
#define SMOCAD_ROUTINGAWARE_H

#include <QProperty>
#include <QVector3D>

#include "Scene/Components/IComponent.h"
#include "Scene/Components/UVParams.h"
#include "Scene/Components/Drawing/Drawing.h"
#include "IntersectionAware.h"

class RoutingAware : public IComponent
{
private:
    bool m_heightmapRendering = false;

public:


    std::shared_ptr<UVParams> p_UV;
    std::shared_ptr<Drawing> p_Drawing;
    std::shared_ptr<IntersectionAware> p_Intersection;

    static std::shared_ptr<RoutingAware>
    CreateRegisteredComponent(unsigned int oid, std::shared_ptr<UVParams> uv, std::shared_ptr<Drawing> draw, std::shared_ptr<IntersectionAware> inter);
    void UnregisterComponent();

    explicit RoutingAware(unsigned int oid);
    ~RoutingAware() override;

    bool IsHeightmapRendering();
    void StartHeighmapRendering();
    void FinishHeighmapRendering();
};

#endif //SMOCAD_ROUTINGAWARE_H
