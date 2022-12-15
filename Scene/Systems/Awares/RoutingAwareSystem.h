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
    enum ZigZagVariable
    {
        X, Y
    };

    enum ZigZagDirection
    {
        Positive, Negative
    };

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
    void DebugSaveConfMap(const std::vector<float> &map, const QString &path, QSize texSize, float blockHeight);

    std::vector<QVector3D> GenerateRoughZigZag(
            const std::vector<float> &confMap, QVector3D startPoint, float w, QVector3D blockSize, QSize texSize,
            float tolerance, ZigZagVariable variable, ZigZagDirection direction);
    std::vector<QVector3D> GenerateFlatZigZag(
            const std::vector<float> &confMap, QVector3D startPoint, float targetWidth, float w, QVector3D blockSize,
            QSize texSize,
            ZigZagVariable variable, bool flipZigZag);
    std::vector<QVector3D> GenerateFlatPrecisionPath(const QVector3D &blockWorldPos, const QVector3D &blockSize);

    std::vector<std::pair<QPoint, float>>
    CreateZigZagPoints(QPoint startPoint, int width, float height, QSize planeSize, ZigZagVariable variable,
                       ZigZagDirection direction);
    std::vector<std::pair<QPoint, float>>
    CreateZigZagPoints(QPoint startPoint, int width, float height, QPoint zigZagBoundaries, int targetWidth,
                       ZigZagVariable variable);
    std::vector<std::vector<QPoint>>
    CreateZigZagLines(QPoint startPoint, int width, QPoint zigZagBoundaries, int targetWidth, ZigZagVariable variable);
    std::vector<QVector3D>
    OptimizeRouterPath(const std::vector<std::pair<QPoint, float>> &zigzag, float tolerance, float maxDepth, QVector3D blockSize,
                       QSize texSize);

    QPoint FromBlockToTex(QVector2D blockPoint, QSize texSize, QVector3D blockSize);
    QVector2D FromTexToBlock(QPoint texPoint, QSize texSize, QVector3D blockSize);
    QVector3D FromSceneToBlock(QVector3D sceneP, QVector3D blockWorldPos);

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
