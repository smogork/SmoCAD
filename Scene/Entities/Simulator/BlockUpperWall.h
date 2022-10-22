//
// Created by ksm on 10/21/22.
//

#ifndef SMOCAD_BLOCKUPPERWALL_H
#define SMOCAD_BLOCKUPPERWALL_H

#include "Scene/Entities/IEntity.h"
#include "Scene/Components/Transform.h"
#include "Scene/Components/Drawing/StaticDrawing.h"

class BlockUpperWall: public IEntity
{
private:
    QMatrix4x4 m_simulatorMtx;
    double m_widthX, m_widthY;
    int m_vertexWidthX, m_vertexWidthY;
    int m_indexCount;
    
    void InitializeDrawing();
    void DrawingFunction(QOpenGLContext* context);
    void UniformFunction(std::shared_ptr<ShaderWrapper> shader);
    std::vector<float> GenerateGeometryVertices();
    std::vector<int> GenerateTopologyIndices();

public:
    std::shared_ptr<Transform> p_Transform;
    std::shared_ptr<StaticDrawing> p_Drawing;
    
    BlockUpperWall(QVector3D pos, double widthX, double widthY, int vertexWidthX, int vertexWidthY);
};

#endif //SMOCAD_BLOCKUPPERWALL_H
