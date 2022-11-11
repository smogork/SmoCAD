//
// Created by ksm on 11/9/22.
//

#ifndef SMOCAD_ROUTINGAWARESYSTEM_H
#define SMOCAD_ROUTINGAWARESYSTEM_H

#include "Scene/Systems/ISystem.h"
#include "Scene/Components/Awares/RoutingAware.h"
#include "glwidget.h"

class RoutingAwareSystem : public ISystem<RoutingAware>
{
private:
    void StartHeighmapRendering(QVector3D blockWorldPos, QVector3D blockSize);
    void FinishHeighmapRendering();
    void RenderHeightmap(QOpenGLContext *context);

    static constexpr float K16_RADIUS = 1.6;
    static constexpr float K8_RADIUS = 0.8;
    static constexpr float F12_RADIUS = 1.2;

public:
    /*QProperty<QVector3D> WorldStartPoint;
    /**
     * (Wx, Wy, H)

    QProperty<QVector3D> BlockSize;*/

    RoutingAwareSystem() : ISystem(SYSTEM_ID::ROUTING_AWARE)
    {}

    const char *GetSystemName() override
    { return "RoutingAwareSystem"; }

    void
    GenerateRoutes3C(GLWidget *gl, const QString& folderName, QVector3D blockWorldPos, QVector3D blockSize,
                     int offscreenSize);

};

#endif //SMOCAD_ROUTINGAWARESYSTEM_H
