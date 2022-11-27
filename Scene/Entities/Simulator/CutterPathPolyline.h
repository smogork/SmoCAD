//
// Created by ksm on 10/21/22.
//

#ifndef SMOCAD_CUTTERPATHPOLYLINE_H
#define SMOCAD_CUTTERPATHPOLYLINE_H

#include "Scene/Entities/IEntity.h"
#include "FileManagers/CutterPath.h"
#include "Scene/Components/Transform.h"
#include "Scene/Components/Drawing/StaticDrawing.h"

class CutterPathPolyline : public IEntity
{
private:
    std::vector<QVector3D> m_points;
    std::weak_ptr<Transform> m_simulatorTransform;
    
    void InitializeDrawing();
    void DrawingFunction(QOpenGLContext *context);
    void UniformFunction(std::shared_ptr<ShaderWrapper> shader);
    std::vector<float> GenerateGeometryVertices();
    std::vector<int> GenerateTopologyIndices();

public:
    std::shared_ptr<StaticDrawing> p_Drawing;
    QColor DrawingColor = Qt::red;
    
    CutterPathPolyline(const std::vector<QVector3D>& points, std::shared_ptr<Transform> simulatorTransform);
};

#endif //SMOCAD_CUTTERPATHPOLYLINE_H
