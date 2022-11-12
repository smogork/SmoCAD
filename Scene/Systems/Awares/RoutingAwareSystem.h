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
    std::unique_ptr<ShaderWrapper> zmapStampCreatorShader;
    std::unique_ptr<ShaderWrapper> zmapAnalizerShader;
    static constexpr float K16_RADIUS = 0.8;
    static constexpr float K8_RADIUS = 0.4;
    static constexpr float F12_RADIUS = 0.6;
    static constexpr float F10_RADIUS = 0.5;

    void StartHeighmapRendering(QVector3D blockWorldPos, QVector3D blockSize);
    void FinishHeighmapRendering();
    void RenderHeightmap(QOpenGLContext *context);

    std::shared_ptr<QOpenGLTexture> CreateStampTexture(
            GLWidget *gl, float radius, int offscreenSize, bool isCylindrical, QVector3D blockSize);
    std::vector<float> CreateConfigurationMap(
            GLWidget *gl, std::shared_ptr<QOpenGLTexture> heightTex, float radius, int offscreenSize,
            bool isCylindrical, QVector3D blockSize);
    void DebugSaveConfMap(const std::vector<float>& map, const QString& path, QSize texSize, float blockHeight);

public:

    RoutingAwareSystem() : ISystem(SYSTEM_ID::ROUTING_AWARE)
    {}
    void ClearSystem() override;

    const char *GetSystemName() override
    { return "RoutingAwareSystem"; }

    void
    GenerateRoutes3C(GLWidget *gl, const QString &folderName, QVector3D blockWorldPos, QVector3D blockSize,
                     int offscreenSize);

};

#endif //SMOCAD_ROUTINGAWARESYSTEM_H
