//
// Created by ksm on 10/20/22.
//

#ifndef SMOCAD_CUTTEROBJECT_H
#define SMOCAD_CUTTEROBJECT_H

#include "Scene/Entities/IEntity.h"
#include "Scene/Components/Transform.h"
#include "Scene/Components/Drawing/DynamicDrawing.h"
#include "CutterParameters.h"

class CutterObject: public IEntity
{
private:
    static constexpr int VertexRCount = 32;
    static constexpr int VertexLCount = 16;
    static constexpr int VertexThetaCount = 16;
    static constexpr int CutterHeight = 4;
    CutterParameters m_params;
    std::weak_ptr<Transform> m_simulatorTransform;
    int m_indexCount;
    
    void InitializeDrawing();
    void DrawingFunction(QOpenGLContext* context);
    void UniformFunction(std::shared_ptr<ShaderWrapper> shader);
    std::vector<float> GenerateGeometryVertices();
    std::vector<int> GenerateTopologyIndices();
    void AppendToVertices(std::vector<float>& vertices, const std::vector<QVector3D> points);

public:
    std::shared_ptr<Transform> p_Transform;
    std::shared_ptr<DynamicDrawing> p_Drawing;
    
    CutterObject(QVector3D pos, CutterParameters params, std::shared_ptr<Transform> simulatorTransform);
    void SetCutterParameters(Length diameter, CutterType type);
    void SetCutterParameters(CutterParameters params);
};

#endif //SMOCAD_CUTTEROBJECT_H
