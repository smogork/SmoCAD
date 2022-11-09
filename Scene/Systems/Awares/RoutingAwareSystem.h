//
// Created by ksm on 11/9/22.
//

#ifndef SMOCAD_ROUTINGAWARESYSTEM_H
#define SMOCAD_ROUTINGAWARESYSTEM_H

#include "Scene/Systems/ISystem.h"
#include "Scene/Components/Awares/RoutingAware.h"

class RoutingAwareSystem : public ISystem<RoutingAware>
{
private:

public:
    QProperty<QVector3D> WorldStartPoint;
    /**
     * (Wx, Wy, H)
     */
    QProperty<QVector3D> BlockSize;

    RoutingAwareSystem() : ISystem(SYSTEM_ID::ROUTING_AWARE),
                           WorldStartPoint({0, 0, 0}),
                           BlockSize({15, 15, 5})
    {}

    const char *GetSystemName() override
    { return "RoutingAwareSystem"; }

    void StartHeighmapRendering();
    void FinishHeighmapRendering();
    void RenderHeightmap(QOpenGLContext *context);

};

#endif //SMOCAD_ROUTINGAWARESYSTEM_H
